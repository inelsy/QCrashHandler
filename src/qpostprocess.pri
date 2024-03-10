#CONFIG(debug, debug|release) {
#  # nothing here
#} else {

#    load(resolve_target)
#    TARGET_FULL = $$basename(QMAKE_RESOLVED_TARGET)
#    TARGET_PATH = $$dirname(QMAKE_RESOLVED_TARGET)
#    TARGET_PDB  = $${TARGET_FULL}.pdb
#    unix {
#        QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; objcopy --only-keep-debug "$${TARGET_FULL}" "$${TARGET_PDB}" $$escape_expand(\n\t))
#        QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; strip --strip-debug --strip-unneeded "$${TARGET_FULL}" $$escape_expand(\n\t))
#        QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; objcopy --add-gnu-debuglink="$${TARGET_PDB}" "$${TARGET_FULL}" $$escape_expand(\n\t))

##       QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; qcrashdumper $$escape_expand(\n\t))

#    }
#    win32 {
##    QMAKE_POST_LINK += $$quote( cmd /c objcopy --only-keep-debug "$${TARGET_PATH}"/"$${TARGET_FULL}" "$${TARGET_PATH}"/"$${TARGET_PDB}" $$escape_expand(\n\t))
##          QMAKE_POST_LINK += $$quote( cmd /c strip --strip-debug --strip-unneeded "$${TARGET_PATH}"/"$${TARGET_FULL}" $$escape_expand(\n\t))
##          QMAKE_POST_LINK += $$quote( cmd /c objcopy --add-gnu-debuglink="$${TARGET_PATH}"/"$${TARGET_PDB}" "$${TARGET_PATH}"/"$${TARGET_FULL}" $$escape_expand(\n\t))
#        QMAKE_POST_LINK += $$quote( cmd /c "$${PWD}"/cv2pdb.exe "$${TARGET_PATH}"/"$${TARGET_FULL}"  $$escape_expand(\n\t))


#    }
#}
