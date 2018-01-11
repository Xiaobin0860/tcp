defmodule TcpTest do
  use ExUnit.Case
  doctest Tcp

  test "greets the world" do
    assert Tcp.hello() == :world
  end

  test "tcp_connection" do
    {:ok, listener} = :gen_tcp.listen(0, [active: false, mode: :binary])
    {:ok, port} = :inet.port(listener)
    {:ok, conn} = TCPConnection.start_link({127,0,0,1}, port, [mode: :binary])
    {:ok, socket} = :gen_tcp.accept(listener, 1000)

    assert :gen_tcp.send(socket, "hello") == :ok
    assert TCPConnection.recv(conn, 5, 1000) == {:ok, "hello"}
    assert TCPConnection.send(conn, "hi") == :ok
    assert :gen_tcp.recv(socket, 2, 1000) == {:ok, "hi"}
    assert TCPConnection.close(conn) == :ok
    assert :gen_tcp.recv(socket, 0, 1000) == {:error, :closed}

    {:ok, socket} = :gen_tcp.accept(listener, 1000)

    assert TCPConnection.send(conn, "back!") == :ok
    assert :gen_tcp.recv(socket, 5, 1000) == {:ok, "back!"}
  end

  # test "tcp_connection to cpp" do
  #   {:ok, conn} = TCPConnection.start_link({127,0,0,1}, 11112, [active: false, packet: 4, mode: :binary])
  #   assert TCPConnection.send(conn, "hi") == :ok
  #   assert TCPConnection.recv(conn, 0) == {:ok, "World"}
  #   assert TCPConnection.close(conn) == :ok
  # end

  # test "client to cpp" do
  #   {:ok, c} = Client.start_link("127.0.0.1", 11112)
  #   assert Client.send(c, "hi") == :ok
  #   assert Client.recv(c) == {:ok, "World"}
  #   assert Client.close(c) == :ok
  # end

  test "server and client" do
    {:ok, s} = Server.start_link(Server, 3333, Handler, [])
    {:ok, c} = Client.start_link("127.0.0.1", 3333)
    assert Client.send(c, "hi") == :ok
    assert Client.recv(c) == {:ok, "hi"}
    assert Client.send(c, "hello") == :ok
    assert Client.recv(c) == {:ok, "hello"}
    assert Client.close(c) == :ok
    assert Server.stop(s) == :ok
  end
end
