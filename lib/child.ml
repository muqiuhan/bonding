open Core

let child (config : Config.t) =
    config.argv
    |> String.concat ~sep:" "
    |> Log.info "Starting container with command %s and args %s" config.path;
    
    0

external __generate_child_process : unit -> int = "__generate_child_process"

let generate_child_process (config : Config.t) =
    let __make : unit -> int = fun () -> child config in
        Callback.register "__make" __make;
        __generate_child_process ()