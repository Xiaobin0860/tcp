use structopt::StructOpt;
use tokio::io::copy;
use tokio::net::TcpListener;
use tokio::prelude::*;

/// A basic example
#[derive(StructOpt, Debug)]
#[structopt(name = "EchoServer")]
struct Opt {
    #[structopt(short, long, default_value = "0.0.0.0:11111")]
    addr: String,
}

fn main() {
    let opt = Opt::from_args();
    // Bind the server's socket.
    let addr = opt.addr.parse().expect("addr format error! ip:port");
    let listener = TcpListener::bind(&addr).expect("unable to bind TCP listener");

    // Pull out a stream of sockets for incoming connections
    let server = listener
        .incoming()
        .map_err(|e| eprintln!("accept failed = {:?}", e))
        .for_each(|sock| {
            // Split up the reading and writing parts of the
            // socket.
            let (reader, writer) = sock.split();

            // A future that echos the data and returns how
            // many bytes were copied...
            let bytes_copied = copy(reader, writer);

            // ... after which we'll print what happened.
            let handle_conn = bytes_copied
                .map(|amt| println!("wrote {:?} bytes", amt))
                .map_err(|err| eprintln!("IO error {:?}", err));

            // Spawn the future as a concurrent task.
            tokio::spawn(handle_conn)
        });

    // Start the Tokio runtime
    tokio::run(server);
}
