package main

import (
	"encoding/binary"
	"errors"
	"io"
	"net"

	"../../3rd/gotcp"
)

const MAX_PKT_LEN uint32 = 1024 * 1024

type Pkt struct {
	buff []byte
}

func (this *Pkt) Serialize() []byte {
	return this.buff
}

func (this *Pkt) GetLength() uint32 {
	return binary.BigEndian.Uint32(this.buff[0:4])
}

func (this *Pkt) GetBody() []byte {
	return this.buff[4:]
}

func NewPkt(buff []byte, hasLengthField bool) *Pkt {
	p := &Pkt{}

	if hasLengthField {
		p.buff = buff

	} else {
		p.buff = make([]byte, 4+len(buff))
		binary.BigEndian.PutUint32(p.buff[0:4], uint32(len(buff)))
		copy(p.buff[4:], buff)
	}

	return p
}

type Proto struct {
}

func (this *Proto) ReadPacket(conn *net.TCPConn) (gotcp.Packet, error) {
	var (
		lengthBytes []byte = make([]byte, 4)
		length      uint32
	)

	// read length
	if _, err := io.ReadFull(conn, lengthBytes); err != nil {
		return nil, err
	}
	if length = binary.BigEndian.Uint32(lengthBytes); length > MAX_PKT_LEN {
		return nil, errors.New("the size of packet is larger than the limit")
	}

	buff := make([]byte, 4+length)
	copy(buff[0:4], lengthBytes)

	// read body ( buff = lengthBytes + body )
	if _, err := io.ReadFull(conn, buff[4:]); err != nil {
		return nil, err
	}

	return NewPkt(buff, true), nil
}
