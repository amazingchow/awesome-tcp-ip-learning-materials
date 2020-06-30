package main

import (
	"context"
	"fmt"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/gorilla/websocket"

	myepoll "github.com/amazingchow/over-the-cx-problem/handle-1m-websocket/epoll"
)

var (
	_Epoll    *myepoll.Epoll
	_Upgrader websocket.Upgrader
	_Stop     chan struct{}
	_Shutdown chan struct{}
)

func webSocketHandler(w http.ResponseWriter, r *http.Request) {
	conn, err := _Upgrader.Upgrade(w, r, nil)
	if err != nil {
		return
	}

	if err := _Epoll.Add(conn); err != nil {
		fmt.Printf("failed to add websocket conn to epoll, err: %v\n", err)
		conn.Close()
	}
}

func start() {
EPOLL_LOOP:
	for {
		select {
		case _, ok := <-_Stop:
			{
				if !ok {
					break EPOLL_LOOP
				}
			}
		default:
			{
				conns, err := _Epoll.Wait()
				if err != nil {
					fmt.Printf("epoll wait failed, err: %v\n", err)
					continue
				}
				for _, conn := range conns {
					if conn == nil {
						continue
					}

					_, _, err = conn.ReadMessage()
					if err != nil {
						if err = _Epoll.Remove(conn); err != nil {
							fmt.Printf("failed to remove websocket conn from epoll, err: %v\n", err)
						}
						conn.Close()
					} else {
						// do sth with the msg
					}
				}
			}
		}
	}

	_Shutdown <- struct{}{}
}

func main() {
	var err error

	_Stop = make(chan struct{})
	_Shutdown = make(chan struct{})

	// increase resource limitation
	var rLimit syscall.Rlimit
	if err = syscall.Getrlimit(syscall.RLIMIT_NOFILE, &rLimit); err != nil {
		panic(err)
	}
	fmt.Printf("Rlimit.Cur = %d, Rlimit.Max=%d\n", rLimit.Cur, rLimit.Max)
	rLimit.Cur = rLimit.Max
	if err = syscall.Setrlimit(syscall.RLIMIT_NOFILE, &rLimit); err != nil {
		panic(err)
	}

	// start epoll - I/O event notification facility
	_Epoll, err = myepoll.MkEpoll()
	if err != nil {
		panic(err)
	}

	go start()

	_Upgrader = websocket.Upgrader{HandshakeTimeout: time.Second * 5}
	http.HandleFunc("/", webSocketHandler)

	// enable pprof hooks
	pprofServer := http.Server{
		Addr: "localhost:6060",
	}
	go func() {
		if err = pprofServer.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			panic(err)
		}
	}()

	// enable websocket server
	websocketServer := http.Server{
		Addr: "localhost:8080",
	}
	go func() {
		if err := websocketServer.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			panic(err)
		}
	}()

	ctx, cancel := context.WithCancel(context.Background())
	defer func() {
		pprofServer.Shutdown(ctx)     // nolint
		websocketServer.Shutdown(ctx) // nolint
		close(_Stop)
		<-_Shutdown
		cancel()
	}()

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, os.Interrupt, syscall.SIGTERM, syscall.SIGINT)
LOOP:
	for { // nolint
		select {
		case <-sigCh:
			{
				break LOOP
			}
		}
	}
}
