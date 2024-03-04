# in case of linux, add post build commands to separate debug symbols
# linux-g++ {
  load(resolve_target)
  TARGET_FULL = $$basename(QMAKE_RESOLVED_TARGET)
  TARGET_PATH = $$dirname(QMAKE_RESOLVED_TARGET)
  TARGET_PDB  = $${TARGET_FULL}.pdb
  QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; cv2pdb "$${TARGET_FULL}" $$escape_expand(\n\t))
  # QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; strip --strip-debug --strip-unneeded "$${TARGET_FULL}" $$escape_expand(\n\t))
  # QMAKE_POST_LINK += $$quote( cd "$${TARGET_PATH}"; objcopy --add-gnu-debuglink="$${TARGET_PDB}" "$${TARGET_FULL}" $$escape_expand(\n\t))
# }
