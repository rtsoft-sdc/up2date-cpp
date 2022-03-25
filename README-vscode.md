# vscode usage

see also https://code.visualstudio.com/docs/remote/remote-overview

SYSTEM REQUIREMENTS:
- docker
- vscode Remote Development extension pack

```F1: Remote-Containers: Reopen in Container```

```F1: CMake: Configure```

```F1: CMake: Build```

How to generate docs:

```F1: Generate Doxygen documentation```

Note: if your host is running Windows git checks out text files autoconvering CR/CRLF lines. So you cannot commit from inside a container, just switch back nad commit your changes:

```F1: Remote-Containers: Reopen Folder Locally```
