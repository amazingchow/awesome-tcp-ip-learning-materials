package main

import (
	"flag"
	"fmt"
	"net/url"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/gorilla/websocket"
)

var (
	hostFlag = flag.String("host", "127.0.0.1:8080", "server address")
	connFlag = flag.Int("conn", 1, "number of websocket connections")
)

func main() {
	flag.Parse()

	u := url.URL{
		Scheme: "ws",
		Host:   *hostFlag,
		Path:   "/",
	}
	fmt.Printf("connecting to %s\n", u.String())

	conns := make([]*websocket.Conn, *connFlag)
	for i := 0; i < *connFlag; i++ {
		conn, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
		if err != nil {
			fmt.Printf("failed to dial to %s, err: %v\n", u.String(), err)
			conns[i] = nil
		} else {
			conns[i] = conn
		}
	}

	fmt.Printf("initialize %d websocket connections\n", len(conns))

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, os.Interrupt, syscall.SIGTERM, syscall.SIGINT)
MAIN_LOOP:
	for {
		select {
		case <-sigCh:
			{
				break MAIN_LOOP
			}
		default:
			{
				for i := 0; i < len(conns); i++ {
					if conns[i] != nil {
						if err := conns[i].WriteControl(websocket.PingMessage, nil, time.Now().Add(time.Second*5)); err != nil {
							fmt.Printf("conn-%d failed to receive pong, err: %v\n", i, err)
							continue
						}
						if err := conns[i].WriteMessage(websocket.TextMessage, []byte(fmt.Sprintf("msg from conn-%d", i))); err != nil {
							fmt.Printf("conn-%d failed to send msg, err: %v\n", i, err)
							continue
						}
					}
				}
			}
		}
	}

	for i := 0; i < len(conns); i++ {
		conns[i].WriteControl(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""), time.Now().Add(time.Second)) // nolint
		conns[i].Close()
	}
}
