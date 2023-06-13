open Core
open Utils

type t = {
  (* Activate debug mode *)
  debug : bool option;
  (* Command to execute inside the container *)
  command : string option;
  (* User id to create inside the container *)
  uid : int option;
  (* Directory to mount as root of the container *)
  mount_dir : string option;
}
(** Raw configuration parameters received from the command line *)

let check_mount_dir (mount_dir : string option) =
    match mount_dir with
    | Some mount_dir -> begin
        match Unix.access mount_dir [`Exists] with
        | Ok _ -> Ok ()
        | Error _ -> Error (Err.Argument_invalid "mount")
      end
    | None -> Error (Err.Argument_invalid "mount")
