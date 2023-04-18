## 简介

主要为了研究Docker，并伴随着对Linux的安全措施，容器，虚拟化等特性的工作原理的研究。

可移植性是指一个软件能够完美的在不同的环境中执行，不出任何问题。容器完成这一目标。

Docker说
> A container is a standard unit of software that packages up code and all its dependencies so the application runs quickly and reliably from one computing environment to another. A Docker container image is a lightweight, standalone, executable package of software that includes everything needed to run an application: code, runtime, system tools, system libraries and settings.

所以容器是一种使软件运行在不需要考虑底层环境兼容性的环境。可以帮助开发一个软件并将其部署到不同的环境中，例如不同的操作系统。

Docker是一种容器化技术，它将应用程序及其依赖项打包到一个容器中，这个容器包含了应用程序所需的所有组件，包括操作系统、库、运行时环境等。这样，无论在哪个环境中运行，容器中的应用程序都可以保持一致，不会受到环境变化的影响，从而解决了可移植性问题。

Docker容器还提供了环境隔离，每个容器都有自己的文件系统、网络和进程空间。所以容器中的应用程序不会受到其他容器或主机上的应用程序的影响，从而保证了应用程序的稳定性和安全性，这似乎和虚拟机有些像，毕竟Docker还说：
> Containers and virtual machines have similar resource isolation and allocation benefits, but function differently because containers virtualize the operating system instead of hardware.

虚拟机（比如JVM）更像是对硬件的抽象（虚拟化），容器不是这么做的，不同的容器使用不同的方案，Docker是应用级别的虚拟化。

可以把容器简单理解为在一些列安全保障手段下与系统隔离的应用程序。

Rust那一套工具链比较统一，不再赘述，我将使用 `rustc 1.70.0-nightly (af06dce64 2023-04-08)`

## 实现
### 命令行参数
我用`structopt`解析命令行参数，这个库比较简单方便，提供的命令行参数样式也比较经典:
```toml
[dependencies]
structopt = "0.3.26"
```

自然而然的参数解析代码：
```rust
#[derive(Debug, StructOpt)]
#[structopt(name = "bonding", about = "crude container")]
pub struct Args {
    /// Activate debug mode
    #[structopt(short, long)]
    debug: bool,

    /// Will be executed command inside the container (with arguments)
    #[structopt(short, long)]
    pub command: String,

    /// Will be create uid inside the container
    #[structopt(short, long)]
    pub uid: u32,

    /// Mount directory as root of the container
    #[structopt(parse(from_os_str), short, long)]
    pub mount: PathBuf,
}

pub fn parse_args() -> Args {
    Args::from_args()
}
```

对于 `mount` 参数，我们将其解析为`OsString`类型。`OsString`是一个跨平台的字符串类型，它可以表示任何操作系统中的字符串。在Rust中，`OsString`通常用于表示文件路径和命令行参数等需要跨平台支持的字符串。

`structopt(short, long)` 会从字段名中自动创建一短一长两个命令行参数，比如 `mount` 就是 `-m` 和 `--mount`。

可以以`cargo run -- --help`看看结果:
```shell
bonding 0.1.0
crude container

USAGE:
    bonding [FLAGS] --command <command> --mount <mount> --uid <uid>

FLAGS:
    -d, --debug      是否开启Debug模式
    -h, --help       Prints help information
    -V, --version    Prints version information

OPTIONS:
    -c, --command <command>    将在容器内执行的带参数命令
    -m, --mount <mount>        容器内作为根目录的外部文件夹
    -u, --uid <uid>            将被创建的uid
```

### 日志
日志模块实现起来也很简单，我只需要让`info` `warning` `error` `debug` 之类的消息显而易见即可，不然全部`println!` 到一起太粗鲁了。

[rust-colog](https://github.com/chrivers/rust-colog) 是个非常简单的日志库，提供最基本的日志支持，我维护了一份具有更丰富的日志样式和信息的[fork](https://github.com/muqiuhan/rust-colog)，本项目将会使用这个fork作为日志支持。

### 错误处理
我使用 `ErrorCode` 结合Rust本身的 `Option` 与 `Result` 处理错误：
```rust
#[derive(Debug)]
pub enum ErrorCode {}

impl fmt::Display for ErrorCode {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match &self {
            _ => write!(f, "{:?}", self),
        }
    }
}
```

操作系统（Linux）程序执行的返回值是一个整数，0为成功，出现异常则是非0数：
```rust
pub fn exit_with_code(res: Result<(), ErrorCode>) {
    match res {
        Ok(_) => {
            debug!("Exit without any error, returning 0");
            exit(0);
        }

        Err(e) => {
            let retcode = e.get_retcode();
            error!("Error on exit:\n\t{}\n\tReturning {}", e, retcode);
            exit(retcode);
        }
    }
}
```

将`ErrorCode`转换成return code可以通过一个简单的模式匹配实现，这里先返回1：
```rust
impl ErrorCode {
    pub fn get_retcode(&self) -> i32 {
        match self {
            _ => 1,
        }
    }
}
```

把这俩集成到 `main` 测试一下：
```rust
fn main() {
    let mut logger = colog::builder();
    logger.filter(None, LevelFilter::Debug);
    logger.init();

    match cli::parse_args() {
        Ok(args) => {
            debug!("{:?}", args);
            exit_with_code(Ok(()))
        }
        Err(e) => {
            error!("Error while parsing arguments:\n\t{}", e);
            exit(e.get_retcode());
        }
    }

    io::stdout().flush().unwrap();
}
```

```shell
cargo run -- -d -c /bin/bash -u 0 -m ./mount

18.4.2023 19:30:086 [?] > Args { debug: true, command: "/bin/bash", uid: 0, mount: "./mount" }
18.4.2023 19:30:086 [?] > Exit without any error, returning 0
```


### 容器结构
命令行解析后的 `Args` 结构体还是太原始了，可以使用一个 `ContainerConfig` 来进一步归纳这些参数，字面意思就是容器的配置：
```rust
#[derive(Clone)]
pub struct ContainerConfig {
    pub argv: Vec<CString>,

    // Path to the binary/executable/script to be executed inside the container
    pub path: CString,

    // The ID of the user within the container.
    // ID = 0 means it is root
    pub uid: u32,

    // The directory path to use as /root in the container.
    pub mount_dir: PathBuf,
}

impl ContainerConfig {
    pub fn new(
        command: String,
        uid: u32,
        mount_dir: PathBuf,
    ) -> Result<ContainerConfig, ErrorCode> {
        ...
    }
}
```

而一个`Container`结构则包含了容器的相关数据，目前只有`ContainerConfig`，所以一个`Container`结构体的骨架：
```rust
pub struct Container {
    config: ContainerConfig,
}

impl Container {
    pub fn new(args: Args) -> Result<Container, ErrorCode> {
        ...
    }

    pub fn create(&mut self) -> Result<(), ErrorCode> {
       ...
    }
    
    /// This function will be called every time before the container exits
    pub fn clean_exit(&mut self) -> Result<(), ErrorCode> {
       ...
    }
}
```

### 环境检查
需要检查操作系统的内核是否支持容器需要使用的所有特性，与系统进行交互并获取信息可以使用 `nix` crate，通过`uname`获取系统信息，这里检查操作系统内核是否大于`MINIMAL_KERNEL_VERSION`(默认是4.8)，以及架构是否是`x86_64`：

```rust
pub fn check_linux_version() -> Result<(), ErrorCode> {
    let uname = unsafe { get_uname() };

    info!("Linux release: {:#?}", uname.release());

    match uname.release().to_str() {
        Some(release) => {
            if let Ok(version) = scan_fmt!(release, "{f}.{}", f32) {
                if version < MINIMAL_KERNEL_VERSION {
                    return Err(ErrorCode::NotSupported(0));
                }
            } else {
                return Err(ErrorCode::ContainerError(0));
            }
        }

        None => error!("unable to get the kernel version of the operating system!"),
    }

    if uname.machine() != "x86_64" {
        return Err(ErrorCode::NotSupported(1));
    }

    Ok(())
}
```


## 参考
- [rust-colog fork](https://github.com/muqiuhan/rust-colog): https://github.com/muqiuhan/rust-colog
- [rust-colog](https://github.com/chrivers/rust-colog): https://github.com/chrivers/rust-colog
- [Docker](https://www.docker.com/): https://www.docker.com/
- [LXC](https://linuxcontainers.org/lxc/introduction/): https://linuxcontainers.org/lxc/introduction/
- [Container in Rust](https://litchipi.github.io/2021/09/20/container-in-rust-part1.html): https://litchipi.github.io/2021/09/20/container-in-rust-part1.html