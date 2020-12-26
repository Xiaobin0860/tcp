use structopt::StructOpt;
use tokio::io;
use tokio::net::TcpListener;

/// A basic example
#[derive(StructOpt, Debug)]
#[structopt(name = "EchoServer")]
struct Opt {
    #[structopt(short, long, default_value = "0.0.0.0:11111")]
    addr: String,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let opt = Opt::from_args();
    let listener = TcpListener::bind(opt.addr).await?;

    loop {
        let (mut socket, _) = listener.accept().await?;

        tokio::spawn(async move {
            let (mut rd, mut wr) = socket.split();

            if io::copy(&mut rd, &mut wr).await.is_err() {
                eprintln!("failed to copy");
            }
        });
    }
}
