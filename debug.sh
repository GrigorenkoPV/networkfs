cd "$(dirname "$0")"
make clean --silent
echo ================ INSTALL ================	&& \
make install DEBUG=YES				&& \
echo ================ MOUNT ==================	&& \
make mount					&& \
echo ================ STUFF ==================	&& \
echo ================ UMOUNT =================	&& \
make umount					&& \
echo ================ REMOVE =================	&& \
make remove
make clean --silent
