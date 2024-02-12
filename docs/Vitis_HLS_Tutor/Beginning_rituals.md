---
sort: 3
---


# Beginning Rituals

Note: This is for resaerch ONLY.

## Ritual 1

### Launch a Palmetto desktop

Step 1: Open a new tab, search "palmetto documentation", and click the first link.

Step 2: Click "Open onDemand" which appears in a black bar at the bottom of the screen:

<div align=center><img src="Images/Open_onDemand.png" alt="drawing" width="600"/></div>

Step 3: Click "Interactive Apps" and choose "Palmetto Desktop" which is in an orange bar at the top of the screen

<div align=center><img src="Images/palmetto.png" alt="drawing" width="600"/></div>

Step 4: Choose the resolution, number of resource chunks, CPU cores per chunk, and amount of memory per chunk.

Notice that: "Interconnect" should be set to "any".

<div align=center><img src="Images/Launch.png" alt="drawing" width="600"/></div>

Step 5: Click "Launch".

Step 6: In the following window, click "Launch Palmetto Desktop".

<div align=center><img src="Images/desktop.png" alt="drawing" width="600"/></div>

## Ritual 2

### Option 1

Open a terminal, run the shell command:

```bash
source /project/twei2/vitis/vitinstall/Vitis/2022.2/settings64.sh
```

### Option 2

To avoid this ritual every time you log in, you can create a file (for example vitis_setup.sh), and add the command to your file. This file gets automatically sourced every time you log in.

```bash
vim vitis_setup.sh
```

In the `vitis_setup.sh` file, we can input the following command:

```bash
#!/bin/bash

source /project/twei2/vitis/vitinstall/Vitis/2022.2/settings64.sh
```

Then, run the shell command:

```bash
source vitis_setup.sh
```

## Ritual 3

We can create a new folder, run the shell commond:

```bash
mkdir vector_add
cd vector_add
```
Run the shell command:

```bash
vitis_hls
```
Then we open the vitis_hls GUI.

