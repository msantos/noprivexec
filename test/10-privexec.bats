#!/usr/bin/env bats

@test "unprivileged process" {
    run noprivexec echo ok
    cat << EOF
--- output
$output
--- output
EOF

    [ "$status" -eq 0 ]
    [ "$output" = "ok" ]
}

@test "privileged process" {
    run noprivexec ping -c 1 8.8.8.8
    cat << EOF
--- output
|$output|
--- output
EOF

    case $(uname -s) in
    Linux)
        [ "$status" -eq 2 ]
        [ "$output" = "ping: socket: Operation not permitted" ]
        ;;
    OpenBSD)
        [ "$status" -eq 127 ]
        [ "$output" = "noprivexec: ping: Permission denied" ]
        ;;
    *)
        echo "unsupported platform"
        exit 1
    esac
}
