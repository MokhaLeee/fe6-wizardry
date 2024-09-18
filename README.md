# FE6-Wizardry

FE6 based wizardry exploration (EA + Lyn bin-hack)

## Quick setup

```bash
# Install sub-modules
cd Tools
git clone https://github.com/MokhaLeee/FE6-CLib.git
git clone https://github.com/StanHash/EventAssembler.git --recursive
git clone https://github.com/StanHash/FE-PyTools.git --recursive

# Install dependencies
sudo apt-get -y install binutils-arm-none-eabi ctags \
    gcc-arm-none-eabi build-essential cmake re2c ghc \
    cabal-install libghc-vector-dev libghc-juicypixels-dev \
    python3-pip pkg-config libpng* bsdiff moreutils

pip install pyelftools PyInstaller tmx six

# Install DevkitPRO
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman
sudo dkp-pacman -S gba-dev
echo "export DEVKITPRO=/opt/devkitpro" >> ~/.bashrc
echo "export DEVKITARM=\${DEVKITPRO}/devkitARM" >> ~/.bashrc
echo "export DEVKITPPC=\${DEVKITPRO}/devkitPPC" >> ~/.bashrc
echo "export PATH=\${DEVKITPRO}/tools/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc

# Build EA
wget https://dot.net/v1/dotnet-install.sh -O dotnet-install.sh
chmod +x ./dotnet-install.sh
sudo ./dotnet-install.sh --channel 6.0
export DOTNET_ROOT=$HOME/.dotnet
export PATH=$PATH:$DOTNET_ROOT:$DOTNET_ROOT/tools
cd Tools/EventAssembler
./build.sh
```

Put FE6 clean rom named `fe6.gba` in the repo directory, then:

```bash
make
```
