open Core
open Utils

type t = {
  (*  The path of the binary / executable / script to execute inside the container *)
  path : string;
  (* The full arguments passed (including the path option) into the commandline. *)
  argv : string list;
  (* The ID of the user inside the container. An ID of 0 means it’s root (administrator). *)
  uid : int;
  (* The path of the directory we want to use as a / root inside our container. *)
  mount_dir : string;
  read : Unix.File_descr.t;
}
(** Output configuration after checking by Cli module *)

let make (command : string) (uid : int) (mount_dir : string) =
    let argv = String.split ~on:' ' command in
    let path = List.hd_exn argv in

    Result.(
      Cli.generate_socketpair () >>| fun (write, read) ->
      ({path; argv; uid; mount_dir; read}, (write, read)))
