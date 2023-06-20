open Core
open Utils

type t = {
  config : Config.t;
  sockets : Unix.File_descr.t * Unix.File_descr.t;
  child_pid : int option;
}

(* minimal kernel version *)
let minimal_kernel_version : string = "6.3.7"

let make (args : Cli.t) =
    Result.(
      Cli.check_mount_dir args.mount_dir >>= fun mount_dir ->
      Config.make args.command args.uid mount_dir >>| fun (config, sockets) ->
      {config; sockets; child_pid = None})

(* function that will handle the container creation process. *)
let create (container : t) =
    Log.info "Creation finished";
    let child_pid = Some (Child.generate_child_process container.config) in
        Ok {container with child_pid}

(** function that will be called before each exit to be sure we stay clean. *)
let clean_exit (container : t) =
    Log.info "Cleaning container";
    let __close_sockets =
        let write, read = container.sockets in
            try
              Unix.close write;
              Unix.close read
            with Unix.Unix_error (_, _, file_descr) ->
              Log.error "Unable to close socket: %s" file_descr
    in
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

let start (container : t) =
    Option.iter container.child_pid ~f:(fun child_pid ->
        Log.debug "Container child PID : %d" child_pid;
        Unix.waitpid_exn (Pid.of_int child_pid));

    Ok container
