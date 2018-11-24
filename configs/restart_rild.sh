#!/system/bin/sh

do_reset_radio() {
  status=$(getprop gsm.status)
  
  # exit if in flight mode
  if [ "$status" == "2" ] ; then
    return
  fi
  
  subId=$(getprop gsm.subid)
  resetCount=$($BB getprop gsm.resetcount)
  if [ -z $resetCount ] ; then
    resetCount=0
  fi
  
  if [ "$subId" != "1" ] ; then
    sleep 10
  fi

  if ! test -f /tmp/.gsmreset.lock ; then
    touch /tmp/.gsmreset.lock
    if [ "$subId" != "1" ] ; then
      stop ril-daemon
      sleep 5
      start ril-daemon
	  setprop gsm.resetcount $(( $resetCount + 1 ))
    fi
	rm -f /tmp/.gsmreset.lock
  fi
  setprop gsm.radioreset false
}

do_reset_radio