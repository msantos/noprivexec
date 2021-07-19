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
    run noprivexec ping 8.8.8.8
    expect="ping: socket: Operation not permitted"
    cat << EOF
--- output
|$output|
---
|$expect|
--- output
EOF

    [ "$status" -eq 2 ]
    [ "$output" = "$expect" ]
}
