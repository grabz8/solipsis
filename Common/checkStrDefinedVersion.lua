#!/usr/local/bin/lua

-- syntax helper function
function syntax()
  print(string.format('syntax: %s <versionDefineMacroName> <versionFile> <minVersionMajor> <minVersionMinor>', arg[0]))
  os.exit(1)
end

-- check args count
if not (#arg == 4) then
  syntax()
end

versionDefineMacroName = arg[1]
versionFile = arg[2]
minVersionMajor = arg[3]
minVersionMinor = arg[4]

f = io.open(versionFile)
if not f then
  print(string.format('Unable to open %s !', versionFile))
  os.exit(1)
end
io.close(f)

major = 0
minor = 0
for line in io.lines(versionFile) do
  match = string.match(line, '#define%s+' .. versionDefineMacroName)
  if match then
    match = string.match(line, '%s+"(%d+)%.%d+"')
    if not match then
      print('Unable to get major version !')
      os.exit(1)
    end
    major = match
    match = string.match(line, '%s+"%d+%.(%d+)"')
    if not match then
      print('Unable to get minor version !')
      os.exit(1)
    end
    minor = match
  end
end

if major < minVersionMajor or
   (major == minVersionMajor and minor < minVersionMinor) then
  print(string.format('Version %s.%s too old !', major, minor))
  os.exit(1)
end
print(string.format('Version %s.%s is OK', major, minor))
