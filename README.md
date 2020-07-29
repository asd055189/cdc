cdc reference and edit from https://github.com/fd0/rabin-cdc
compile command
`make`
`gcc -o hash hash.c -L/usr/local/opt/openssl/lib -lcrypto -I/usr/local/opt/openssl/include/`
usage
`./rabin-cdc < "file to be chunked"`
`./hash < "file to be deduped"`