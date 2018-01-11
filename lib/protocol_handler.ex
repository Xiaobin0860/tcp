defmodule ProtocolHandler do
  @moduledoc """
  A GenServer that accepts connections on a server and processes messages.
  """

  @default_timeout 20_000

  @typedoc "A module that implements the :ranch_transport behaviour"
  @type transport :: :ranch_tcp

  @typedoc "Transport-specific options"
  @type transport_opts :: :ranch_tcp.opts

  require Logger

  @spec start_link(reference, port, transport, {module, module, transport_opts}) :: GenServer.on_start
  def start_link(ref, socket, transport, {server_module, handler_module, transport_opts}) do
    pid = spawn_link(__MODULE__, :init, [ref, socket, transport, server_module, handler_module, transport_opts])
    {:ok, pid}
  end

  @spec init(reference, port, :ranch_tcp, module, module, :ranch_tcp.opts) :: :ok | no_return
  def init(ref, socket, :ranch_tcp = transport, server_module, handler_module, tcp_opts) do
    :ok = :ranch.accept_ack(ref)

    {recv_timeout, tcp_opts} = Keyword.pop(tcp_opts, :recv_timeout, @default_timeout)

    transport_options = Keyword.put(tcp_opts, :packet, 4)
    transport.setopts(socket, transport_options)

    do_call({transport, socket, server_module, handler_module, recv_timeout})
  end

  defp do_call({transport, socket, _server_module, _handler_module, recv_timeout} = args) do
    with {:ok, message} <- transport.recv(socket, 0, recv_timeout)
    do
      Logger.debug("recv #{message}")
      transport.send(socket, message)
      do_call(args)
    else
      {:error, {:server_error, data}} ->
        :ok = transport.send(socket, data)
        exit({:shutdown, :server_error})

      {:error, _} = err ->
        Logger.info(inspect err)
        :ok = transport.close(socket)
    end
  end
end
