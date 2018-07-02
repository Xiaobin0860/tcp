package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"time"

	"../../3rd/glog"
	"../proto"
)

func main() {
	flag.Parse()
	defer func() {
		fmt.Println("Message in defer")
	}()
	defer glog.Flush()
	fmt.Printf("Temp folder for log files: %s\n", os.TempDir())

	glog.Info("Hello Info")
	glog.Warning("Hello Warning")
	glog.Error("Hello Error")
	glog.V(5).Info("hello v5")
	glog.V(0).Info("hello v0")
	glog.V(1).Info("hello v1")
	glog.V(2).Info("hello v2")
	glog.V(3).Info("hello v3")
	glog.V(4).Info("hello v4")
	//glog.Fatal("Hello Fatal")

	tcpAddr, err := net.ResolveTCPAddr("tcp4", "127.0.0.1:11112")
	checkError(err)
	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	checkError(err)

	echoProtocol := &proto.EchoProtocol{}

	// ping <--> pong
	for i := 0; i < 3; i++ {
		// write
		conn.Write(proto.NewEchoPacket([]byte("hello"), false).Serialize())

		// read
		p, err := echoProtocol.ReadPacket(conn)
		if err == nil {
			echoPacket := p.(*proto.EchoPacket)
			fmt.Printf("Server reply:[%v] [%v]\n", echoPacket.GetLength(), string(echoPacket.GetBody()))
		}

		time.Sleep(2 * time.Second)
	}

	conn.Close()
}

func checkError(err error) {
	if err != nil {
		glog.Fatal(err)
	}
}
