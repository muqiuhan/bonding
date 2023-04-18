use std::path::PathBuf;
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(name = "bonding", about = "crude container")]
pub struct Args {
    /// 是否开启Debug模式
    #[structopt(short, long)]
    debug: bool,

    /// 将在容器内执行的带参数命令
    #[structopt(short, long)]
    pub command: String,

    /// 将被创建的uid
    #[structopt(short, long)]
    pub uid: u32,

    /// 容器内作为根目录的外部文件夹
    #[structopt(parse(from_os_str), short, long)]
    pub mount: PathBuf,
}

pub fn parse_args() -> Args {
    Args::from_args()
}
