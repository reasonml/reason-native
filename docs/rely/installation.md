---
id: installation
title: Installing Rely
sidebar_label: Installation
---

We recommend that you use [`esy`](https://esy.sh/) to handle your package management. To install esy using npm, run
```bash
npm install -g esy
```

Add it as a dependency to your package.json (or esy.json) and run ```esy install```. If you don't want to distribute your tests as part of your release, you can utilize  [multiple sandboxes](https://esy.sh/docs/en/multiple-sandboxes.html) and .opam files to separate your dependencies

**package.json**
```
...

dependencies": {
    ...
    "@reason-native/rely": "*",
    ...
},
...
```
