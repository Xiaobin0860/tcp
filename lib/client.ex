defmodule Client do

  @immutable_tcp_opts [active: false, packet: 4, mode: :binary]

  require Logger
  use Connection

  #-------------------------------------------------------------------------
  def start_link(host, port, opts \\ [], timeout \\ 5000) do
    Connection.start_link(__MODULE__, {host, port, opts, timeout})
  end

  def send(conn, data), do: Connection.call(conn, {:send, data})

  def recv(conn, timeout \\ 3000) do
    Connection.call(conn, {:recv, timeout})
  end

  def close(conn), do: Connection.call(conn, :close)
  #-------------------------------------------------------------------------

  def init({host, port, opts, timeout}) do
    s = %{host: to_host(host), port: port, opts: opts, timeout: timeout, sock: nil}
    {:connect, :init, s}
  end

  def connect(_, %{sock: nil, host: host, port: port, opts: opts, timeout: timeout} = s) do
    opts = opts
    |> Keyword.merge(@immutable_tcp_opts)
    |> Keyword.put_new(:send_timeout, 1000)
    case :gen_tcp.connect(host, port, opts, timeout) do
      {:ok, sock} ->
        {:ok, %{s | sock: sock}}
      {:error, _} ->
        {:backoff, 1000, s}
    end
  end

  def disconnect(info, %{sock: sock} = s) do
    :ok = :gen_tcp.close(sock)
    case info do
      {:close, from} ->
        Connection.reply(from, :ok)
      {:error, :closed} ->
        :error_logger.format("Connection closed~n", [])
      {:error, reason} ->
        reason = :inet.format_error(reason)
        :error_logger.format("Connection error: ~s~n", [reason])
    end
    {:connect, :reconnect, %{s | sock: nil}}
  end

  def handle_call(_, _, %{sock: nil} = s) do
    {:reply, {:error, :closed}, s}
  end
  def handle_call({:send, data}, _, %{sock: sock} = s) do
    case :gen_tcp.send(sock, data) do
      :ok ->
        {:reply, :ok, s}
      {:error, _} = error ->
        {:disconnect, error, error, s}
    end
  end
  def handle_call({:recv, timeout}, _, %{sock: sock} = s) do
    case :gen_tcp.recv(sock, 0, timeout) do
      {:ok, _} = ok ->
        {:reply, ok, s}
      {:error, :timeout} = timeout ->
        {:reply, timeout, s}
      {:error, _} = error ->
        {:disconnect, error, error, s}
    end
  end
  def handle_call(:close, from, s) do
    {:disconnect, {:close, from}, s}
  end

  defp to_host(host) when is_bitstring(host) do
    String.to_charlist(host)
  end
  defp to_host(host) when is_list(host), do: host
end
