open Core

type t = {config : Config.t}

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
