# FileSystem
VFS using Fuse

# What is FUSE?

    ->VFS and FUSE are related, but not quite the same thing. The main purpose of FUSE is to turn things-that-are-almost-like-files-but-not-quite (such as files on a remote server, or inside a ZIP file) into "real" directories and files. See the roster of FUSE filesystems to get an idea of what this is good for; this hopefully will make it clearer why FUSE beats "plain old files" in a lot of circumstances.
    ->It provides a way to intercept file system calls issued by Linux programs and to redirect
    the program flow into a handler (daemon running as a user-level process) function.
    ->It also helps to mount the file system.
    ->FUSE helps you in writing a virtual file system.
    


