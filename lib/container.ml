open Core

type t = {config : Config.t}

(* minimal kernel version *)
let minimal_kernel_version : string = "6.3.6"

let make (args : Cli.t) =
    Result.bind
      ~f:(fun mount_dir ->
        Ok {config = Config.make args.command args.uid mount_dir})
      (Cli.check_mount_dir args.mount_dir)

(* function that will handle the container creation process. *)
let create (container : t) =
    Log.info "Creation finished";
    Ok container

(** function that will be called before each exit to be sure we stay clean. *)
let clean_exit (_container : t) =
    Log.info "Cleaning container";
    Log.info "Finished, clean & exit";
    Ok ()

(** check kernel version *)
let check_linux_version () =
    let release, machine =
        let uname = Core_unix.uname () in
        let release =
            uname |> Core_unix.Utsname.release |> fun release ->
            Scanf.sscanf release "%f.%d" (fun f d -> Format.sprintf "%g.%d" f d)
        and machine = uname |> Core_unix.Utsname.machine in
            (release, machine)
    in
        try
          let kernel_release = release in
              Log.debug "Linux release : %s" kernel_release;

              if String.equal kernel_release minimal_kernel_version then
                if String.equal machine "x86_64" then
                  Ok ()
                else
                  Error (Err.Not_supported 0)
              else
                Error (Err.Not_supported 0)
        with _ -> Error (Err.Container_error 1)
