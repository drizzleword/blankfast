<!--
Copyright 2024 Rùnag project contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

# Blankfast

Blankfast overwrites with zeros all 4K-blocks that contains non-zero values in a given regular file or block device.

It could be used to quick nullify flash storage media, as it write speed is usually slower than read speed.

As a bonus, certain flash wear reduction is gained.

Please be aware that this technique is by no means a secure method to erase your data due to wear leveling algorithms used by flash storage controllers.

Before you proceed please understand that this tool will destroy your data and it is expected of it.

# Install

```sh
make
sudo make install # will install in /usr/local/bin
```

# Usage

```sh
blankfast /dev/some-block-device-you-want-to-erase
blankfast some-file-you-want-to-erase
```
