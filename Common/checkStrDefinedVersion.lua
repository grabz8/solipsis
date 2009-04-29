#!/usr/local/bin/lua

-- syntax helper function
function syntax()
  print(string.format('syntax: %s <versionDefineMacroName> <versionFile> <minVersionMajor> <minVersionMinor> <minVersionPatch> [<outputVersionFile>]', arg[0]))
  os.exit(1)
end

-- check args count
if (#arg < 5) then
  syntax()
end

versionDefineMacroName = arg[1]
versionFile = arg[2]
minVersionMajor = arg[3]
minVersionMinor = arg[4]
minVersionPatch = arg[5]
outputVersionFile = arg[6]

f = io.open(versionFile)
if not f then
  print(string.format('Unable to open %s !', versionFile))
  os.exit(1)
end
f:close()

major = 0
minor = 0
patch = 0
for line in io.lines(versionFile) do
  match = string.match(line, '#define%s+' .. versionDefineMacroName)
  if match then
    match = string.match(line, '%s+"(%d+)%.%d%d+"')
    if not match then
      print('Unable to get major version !')
      os.exit(1)
    end
    major = match
    match = string.match(line, '%s+"%d+%.(%d)%d+"')
    if not match then
      print('Unable to get minor version !')
      os.exit(1)
    end
    minor = match
    match = string.match(line, '%s+"%d+%.%d(%d+)"')
    if not match then
      print('Unable to get patch version !')
      os.exit(1)
    end
    patch = match
  end
end

if major < minVersionMajor or
   (major == minVersionMajor and minor < minVersionMinor) or
   (major == minVersionMajor and minor == minVersionMinor and patch < minVersionPatch) then
  print(string.format('Version %s.%s%s too old !', major, minor, patch))
  os.exit(1)
end
print(string.format('Version %s.%s%s is OK', major, minor, patch))

if outputVersionFile then
  writeOutputVersionFile = true
  f = io.open(outputVersionFile)
  if f then
    f:close()
    omajor = 0
    ominor = 0
    opatch = 0
    for line in io.lines(outputVersionFile) do
      match = string.match(line, 'RAKNET_VERSION_MAJOR%s+(%d+)')
      if match then
        omajor = match
      end
      match = string.match(line, 'RAKNET_VERSION_MINOR%s+(%d+)')
      if match then
        ominor = match
      end
      match = string.match(line, 'RAKNET_VERSION_PATCH%s+(%d+)')
      if match then
        opatch = match
      end
    end
    if omajor == major and ominor == minor and opatch == patch then
      writeOutputVersionFile = false
    end
  end
  if writeOutputVersionFile then
    f = io.open(outputVersionFile, "w")
    if not f then
      print(string.format('Unable to create output version file %s !', outputVersionFile))
      os.exit(1)
    end
    f:write('/**\n')
    f:write(string.format(' * Header file automatically created by pre-built LUA script %s\n', arg[0]))
    f:write('**/\n')
    f:write('\n')
    f:write(string.format('#define RAKNET_VERSION_MAJOR %s\n', major))
    f:write(string.format('#define RAKNET_VERSION_MINOR %s\n', minor))
    f:write(string.format('#define RAKNET_VERSION_PATCH %s\n', patch))
    f:close(f)
    print(string.format('Output version file written into %s', outputVersionFile, minor))
  end
end