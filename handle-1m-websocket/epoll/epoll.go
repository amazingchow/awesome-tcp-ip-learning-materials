package epoll

import (
	"fmt"
	"reflect"
	"sync"
	"syscall"

	"github.com/gorilla/websocket"
	"golang.org/x/sys/unix"
)

// More info: https://man7.org/linux/man-pages/man7/epoll.7.html

type Epoll struct {
	mu    sync.RWMutex
	fd    int
	conns map[int]*websocket.Conn
}

func MkEpoll() (*Epoll, error) {
	fd, err := unix.EpollCreate1(0)
	if err != nil {
		return nil, err
	}
	return &Epoll{
		fd:    fd,
		conns: make(map[int]*websocket.Conn),
	}, nil
}

func (ep *Epoll) Add(conn *websocket.Conn) error {
	fd := websocket2fd(conn)

	event := &unix.EpollEvent{
		Events: unix.POLLIN | unix.POLLHUP,
		Fd:     int32(fd),
	}
	err := unix.EpollCtl(ep.fd, syscall.EPOLL_CTL_ADD, fd, event)
	if err != nil {
		return err
	}

	ep.mu.Lock()
	defer ep.mu.Unlock()

	ep.conns[fd] = conn
	if len(ep.conns)%100 == 0 {
		fmt.Printf("total websocket connections: %d\n", len(ep.conns))
	}

	return nil
}

func (ep *Epoll) Remove(conn *websocket.Conn) error {
	fd := websocket2fd(conn)

	err := unix.EpollCtl(ep.fd, syscall.EPOLL_CTL_DEL, fd, nil)
	if err != nil {
		return err
	}

	ep.mu.Lock()
	defer ep.mu.Unlock()

	delete(ep.conns, fd)
	if len(ep.conns)%100 == 0 {
		fmt.Printf("total websocket connections: %d\n", len(ep.conns))
	}

	return nil
}

func (ep *Epoll) Wait() ([]*websocket.Conn, error) {
	events := make([]unix.EpollEvent, 100)
	n, err := unix.EpollWait(ep.fd, events, 100)
	if err != nil {
		return nil, err
	}

	ep.mu.RLock()
	defer ep.mu.RUnlock()

	var conns []*websocket.Conn
	for i := 0; i < n; i++ {
		conns = append(conns, ep.conns[int(events[i].Fd)])
	}
	return conns, nil
}

func websocket2fd(conn *websocket.Conn) int {
	connVal := reflect.Indirect(reflect.ValueOf(conn)).FieldByName("conn").Elem()
	tcpConn := reflect.Indirect(connVal).FieldByName("conn")
	fdVal := tcpConn.FieldByName("fd")
	pfdVal := reflect.Indirect(fdVal).FieldByName("pfd")
	return int(pfdVal.FieldByName("Sysfd").Int())
}
