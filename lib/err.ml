(** Contains all possible errors in our tool *)
type t =
  | Argument_invalid of string
  | Not_supported of int
  | Container_error of int
  | Socket_error of int

let to_string = function
    | Argument_invalid str -> Format.sprintf "Argument invalid : %s" str
    | Not_supported code -> Format.sprintf "Not supported : %d" code
    | Container_error code -> Format.sprintf "Container error : %d" code
    | Socket_error code -> Format.sprintf "Socket error : %d" code

(** Translate an Errcode::X into a number to return (the Unix way) *)
let get_ret_code (_code : t) : int = 1

(** Get the result from a function, and exit the process with the correct code *)
let exit_with_retcode (res : (unit, t) result) : 'a =
    match res with
    (* If there's an error, print an error message and return the retcode *)
    | Error e ->
        let ret_code = get_ret_code e in
            Log.error "Error on exit:\n\t%s\n\tReturning %d" (to_string e)
              ret_code;
            exit ret_code
    (* if its success, return o *)
    | Ok _ ->
        Log.info "Exit without any error, returning 0";
        exit 0
