export BOOST_HOME=/home/td/boost1640
export OPENCV_HOME=/home/td/opencv320
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${OPENCV_HOME}/lib/pkgconfig
export LD_LIBRARY_PATH=${BOOST_HOME}/lib/debug:${PROROOT}/lib:${OPENCV_HOME}/lib:$LD_LIBRARY_PATH

alias cds='cd $PROROOT/src'
alias cdi='cd $PROROOT/include'
alias cdm='cd $PROROOT/mak'
alias cde='cd $PROROOT/etc'
alias cdb='cd $PROROOT/bin'
alias cdp='cd $PROROOT'
alias cdl='cd $PROROOT/lib'
alias cpmake='cp $PROROOT/mak/make.sh .'

ulimit -c unlimited
