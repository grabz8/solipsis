import xml.dom
import xml.dom.minidom
import io
import os
import tempfile
import shutil
import re

def fixNodeEndLines( xmlNode ):
	#print('fixing node '+xmlNode.nodeName)
	if xmlNode.attributes != None:
		for attrIndex in range(0, xmlNode.attributes.length-1):
			attr = xmlNode.attributes.item(attrIndex)
			stringValue = attr.nodeValue
			fixedStringValue = re.sub('\015', 'ampersand#x0D;', stringValue) # 0x0d is 015 in octal
			fixedStringValue = re.sub('\012', 'ampersand#x0A;', fixedStringValue) # 0x0a is 012 in octal
			'''
			if stringValue != fixedStringValue:
				print('fixed string : '+fixedStringValue)
			'''
			attr.nodeValue = fixedStringValue
	for child in xmlNode.childNodes:
		fixNodeEndLines( child )

def fixEndLines( dom ):
	fixNodeEndLines( dom.documentElement )

def putEndLinesBack( xmlAsString ):
	fixedString = re.sub('ampersand#x0D;', '&#x0D;', xmlAsString)
	fixedString = re.sub('ampersand#x0A;', '&#x0A;', fixedString)
	return fixedString
	
def ParseXmlFile( xmlFilePath ):
	dom = xml.dom.minidom.parse( xmlFilePath )
	#fixEndLines(dom)
	return dom

def DomToXmlString( dom ):
	xmlString = dom.toxml("Windows-1252")
	xmlString = putEndLinesBack(xmlString)
	return xmlString
	
def testFix():
	sourceXmlFilePath = 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/peerPhysX_vc8.vcproj'
	destXmlFilePath = 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/peerPhysX_vc8_output.vcproj'
	dom = ParseXmlFile(sourceXmlFilePath)
	xmlString = DomToXmlString(dom)
	file = io.open( destXmlFilePath, 'wb' )
	file.writelines( xmlString )
	file.close()

def CreateIndentation( indentationDepth ):
	string = ''
	for i in range(0, indentationDepth):
		string += '\t'
	return string
	
def GetAttribute( element, attributeName):
	attribute = element.attributes[ attributeName ]
	if attribute == None:
		raise NameError('Malformed Vc8 project file : Unable to find the attribute named '+attributeName)
	return attribute

	
def SerializeAttribute( element, attributeName, indentationDepth ):
	attribute = GetAttribute( element, attributeName )
	attrValue = attribute.nodeValue
	attrValue = re.sub('"', '&quot;', attrValue)
	attrValue = re.sub('\'', '&apos;', attrValue)
	attrValue = re.sub('\015', '&#x0D;', attrValue)	# 0x0d is 015 in octal
	attrValue = re.sub('\012', '&#x0A;', attrValue)	# 0x0a is 012 in octal
	xmlAsString = CreateIndentation( indentationDepth ) + attributeName + '=\"' + attrValue + '\"\n'
	return xmlAsString

def SerializeAttributeIfExists( element, attributeName, indentationDepth ):
	xmlAsString = ''
	attribute = element.getAttributeNode( attributeName )
	if attribute != None: 
		xmlAsString = SerializeAttribute( element, attributeName, indentationDepth )
	return xmlAsString

def FindNamedChildElement( element, childElementName ):
	for child in element.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			if child.tagName == childElementName:
				return child
	return None
	
def SerializePlatformElement( platformElement ):
	if platformElement.tagName != 'Platform':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ platformElement.tagName)
	xmlAsString = CreateIndentation( 2 ) + '<Platform\n'
	xmlAsString += SerializeAttribute( platformElement, "Name", 3 )
	xmlAsString += CreateIndentation( 2 ) + '/>\n'
	return xmlAsString

def SerializePlatformsElement( platformsElement ):
	if platformsElement.tagName != 'Platforms':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ platformsElement.tagName)
	xmlAsString = '\t<Platforms>\n'
	for child in platformsElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			xmlAsString += SerializePlatformElement( child )
	xmlAsString += '\t</Platforms>\n'
	return xmlAsString

def SerializeGenericEmptyElement( elementNode, elementName ):
	if elementNode.tagName != elementName:
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ elementNode.tagName)
	xmlAsString = '\t<'+elementName+'>\n'
	xmlAsString += '\t</'+elementName+'>\n'
	return xmlAsString
	
def SerializeToolFilesElement( toolFilesElement ):
	xmlAsString = SerializeGenericEmptyElement( toolFilesElement, 'ToolFiles' )
	return xmlAsString

def SerializeReferencesElement( referencesElement ):
	xmlAsString = SerializeGenericEmptyElement( referencesElement, 'References' )
	return xmlAsString

def SerializeToolElement( toolElement, indentationDepth ):
	if toolElement.tagName != 'Tool':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ toolElement.tagName)
	xmlAsString = CreateIndentation( indentationDepth ) + '<Tool\n'
	xmlAsString += SerializeAttribute( toolElement, "Name", indentationDepth+1 )
	nameAttribute = GetAttribute( toolElement, "Name" )
	toolName = nameAttribute.nodeValue
	if toolName == 'VCCLCompilerTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalOptions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UseUnicodeResponseFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Optimization", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "InlineFunctionExpansion", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EnableIntrinsicFunctions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FavorSizeOrSpeed", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OmitFramePointers", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EnableFiberSafeOptimizations", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "WholeProgramOptimization", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalIncludeDirectories", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalUsingDirectories", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "PreprocessorDefinitions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreStandardIncludePath", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GeneratePreprocessedFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "KeepComments", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "StringPooling", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MinimalRebuild", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ExceptionHandling", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "BasicRuntimeChecks", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SmallerTypeCheck", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "RuntimeLibrary", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "StructMemberAlignment", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "BufferSecurityCheck", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EnableFunctionLevelLinking", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EnableEnhancedInstructionSet", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FloatingPointModel", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FloatingPointExceptions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DisableLanguageExtensions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DefaultCharIsUnsigned", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TreatWChar_tAsBuiltInType", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ForceConformanceInForLoopScope", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "RuntimeTypeInfo", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OpenMP", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UsePrecompiledHeader", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "PrecompiledHeaderThrough", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "PrecompiledHeaderFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ExpandAttributedSource", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AssemblerOutput", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AssemblerListingLocation", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ObjectFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ProgramDataBaseFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GenerateXMLDocumentationFiles", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "XMLDocumentationFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "BrowseInformation", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "BrowseInformationFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "WarningLevel", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "WarnAsError", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SuppressStartupBanner", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Detect64BitPortabilityProblems", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DebugInformationFormat", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CallingConvention", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CompileAs", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DisableSpecificWarnings", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ForcedIncludeFiles", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ForcedUsingFiles", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ShowIncludes", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UndefinePreprocessorDefinitions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UndefineAllPreprocessorDefinitions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UseFullPaths", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OmitDefaultLibName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ErrorReporting", indentationDepth+1 )
	elif toolName == 'VCLinkerTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "RegisterOutput", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreImportLibrary", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "LinkLibraryDependencies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UseLibraryDependencyInputs", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "UseUnicodeResponseFiles", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalDependencies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ShowProgress", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OutputFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Version", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "LinkIncremental", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SuppressStartupBanner", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalLibraryDirectories", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GenerateManifest", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ManifestFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalManifestDependencies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreAllDefaultLibraries", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreDefaultLibraryNames", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ModuleDefinitionFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AddModuleNamesToAssembly", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EmbedManagedResourceFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ForceSymbolReferences", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DelayLoadDLLs", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AssemblyLinkResource", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MidlCommandFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreEmbeddedIDL", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MergedIDLBaseFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TypeLibraryFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TypeLibraryResourceID", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GenerateDebugInformation", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AssemblyDebug", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ProgramDatabaseFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "StripPrivateSymbols", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GenerateMapFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MapFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MapExports", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SubSystem", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "HeapReserveSize", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "HeapCommitSize", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "StackReserveSize", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "StackCommitSize", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "LargeAddressAware", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TerminalServerAware", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SwapRunFromCD", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SwapRunFromNet", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Driver", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OptimizeReferences", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EnableCOMDATFolding", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OptimizeForWindows98", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FunctionOrder", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "LinkTimeCodeGeneration", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ProfileGuidedDatabase", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "EntryPointSymbol", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ResourceOnlyDLL", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SetChecksum", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "BaseAddress", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "RandomizedBaseAddress", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FixedBaseAddress", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DataExecutionPrevention", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TurnOffAssemblyGeneration", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SupportUnloadOfDelayLoadedDLL", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ImportLibrary", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MergeSections", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TargetMachine", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "FixedBaseAddress", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "KeyFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "KeyContainer", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DelaySign", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AllowIsolation", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Profile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CLRThreadAttribute", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CLRImageType", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ErrorReporting", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CLRUnmanagedCodeCheck", indentationDepth+1 )
	elif toolName == 'VCLibrarianTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalOptions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalDependencies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OutputFile", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SuppressStartupBanner", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalLibraryDirectories", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreDefaultLibraryNames", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "IgnoreAllDefaultLibraries", indentationDepth+1 )
	elif toolName == 'VCPreBuildEventTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "Description", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CommandLine", indentationDepth+1 )
	elif toolName == 'VCPostBuildEventTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "Description", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CommandLine", indentationDepth+1 )
	elif toolName == 'VCMIDLTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "PreprocessorDefinitions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "MkTypLibCompatible", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "SuppressStartupBanner", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TargetEnvironment", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "GenerateStublessProxies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "TypeLibraryName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "HeaderFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "DLLDataFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "InterfaceIdentifierFileName", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "ProxyFileName", indentationDepth+1 )
	elif toolName == 'VCResourceCompilerTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "PreprocessorDefinitions", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Culture", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalIncludeDirectories", indentationDepth+1 )
	elif toolName == 'VCBscMakeTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "SuppressStartupBanner", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "OutputFile", indentationDepth+1 )
	elif toolName == 'VCCustomBuildTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "Description", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CommandLine", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "AdditionalDependencies", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "Outputs", indentationDepth+1 )
	elif toolName == 'VCPreLinkEventTool':
		xmlAsString += SerializeAttributeIfExists( toolElement, "Description", indentationDepth+1 )
		xmlAsString += SerializeAttributeIfExists( toolElement, "CommandLine", indentationDepth+1 )
	xmlAsString += CreateIndentation( indentationDepth ) + '/>\n'
	return xmlAsString

def SerializeConfigurationElement( configurationElement ):
	if configurationElement.tagName != 'Configuration':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ configurationElement.tagName)
	xmlAsString = CreateIndentation( 2 ) + '<Configuration\n'
	xmlAsString += SerializeAttribute( configurationElement, "Name", 3 )
	xmlAsString += SerializeAttribute( configurationElement, "OutputDirectory", 3 )
	xmlAsString += SerializeAttribute( configurationElement, "IntermediateDirectory", 3 )
	xmlAsString += SerializeAttribute( configurationElement, "ConfigurationType", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "InheritedPropertySheets", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "UseOfMFC", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "UseOfATL", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "ATLMinimizesCRunTimeLibraryUsage", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "CharacterSet", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "ManagedExtensions", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "DeleteExtensionsOnClean", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "WholeProgramOptimization", 3 )
	xmlAsString += SerializeAttributeIfExists( configurationElement, "BuildLogFile", 3 )
	
	xmlAsString += CreateIndentation( 3 ) + '>\n'
	for child in configurationElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			xmlAsString += SerializeToolElement( child, 3 )
	xmlAsString += CreateIndentation( 2 ) + '</Configuration>\n'
	return xmlAsString

def SerializeConfigurationsElement( configurationsElement ):
	if configurationsElement.tagName != 'Configurations':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ configurationsElement.tagName)
	xmlAsString = CreateIndentation( 1 ) + '<Configurations>\n'
	for child in configurationsElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			xmlAsString += SerializeConfigurationElement( child )
	xmlAsString += CreateIndentation( 1 ) + '</Configurations>\n'
	return xmlAsString

def SerializeFileConfigurationElement( fileConfElement, indentationDepth ):
	if fileConfElement.tagName != 'FileConfiguration':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ fileConfElement.tagName)
	xmlAsString = CreateIndentation( indentationDepth ) + '<FileConfiguration\n'
	xmlAsString += SerializeAttribute( fileConfElement, "Name", indentationDepth+1 )
	xmlAsString += SerializeAttributeIfExists( fileConfElement, "ExcludedFromBuild", indentationDepth+1 )
	xmlAsString += CreateIndentation( indentationDepth+1 ) + '>\n'
	for child in fileConfElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			if child.tagName == 'Tool':
				xmlAsString += SerializeToolElement( child, indentationDepth+1 )
			else:
				raise NameError('Malformed Vc8 project file : unexpected tag in Filter :'+ child.tagName)
	xmlAsString += CreateIndentation( indentationDepth ) + '</FileConfiguration>\n'
	return xmlAsString

def SerializeFileElement( fileElement, indentationDepth ):
	if fileElement.tagName != 'File':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ fileElement.tagName)
	xmlAsString = CreateIndentation( indentationDepth ) + '<File\n'
	xmlAsString += SerializeAttribute( fileElement, "RelativePath", indentationDepth+1 )
	xmlAsString += SerializeAttributeIfExists( fileElement, "DeploymentContent", indentationDepth+1 )
	xmlAsString += CreateIndentation( indentationDepth+1 ) + '>\n'
	for child in fileElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			if child.tagName == 'FileConfiguration':
				xmlAsString += SerializeFileConfigurationElement( child, indentationDepth+1 )
			else:
				raise NameError('Malformed Vc8 project file : unexpected tag in Filter :'+ child.tagName)
	xmlAsString += CreateIndentation( indentationDepth ) + '</File>\n'
	return xmlAsString

def SerializeFileFilterElement( filterElement, indentationDepth ):
	if filterElement.tagName != 'Filter':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ filterElement.tagName)
	xmlAsString = CreateIndentation( indentationDepth ) + '<Filter\n'
	xmlAsString += SerializeAttribute( filterElement, "Name", indentationDepth+1 )
	xmlAsString += SerializeAttributeIfExists( filterElement, "Filter", indentationDepth+1 )
	xmlAsString += SerializeAttributeIfExists( filterElement, "SourceControlFiles", indentationDepth+1 )
	xmlAsString += SerializeAttributeIfExists( filterElement, "UniqueIdentifier", indentationDepth+1 )
	xmlAsString += CreateIndentation( indentationDepth+1 ) + '>\n'
	for child in filterElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			if child.tagName == 'Filter':
				xmlAsString += SerializeFileFilterElement( child, indentationDepth+1 )
			elif child.tagName == 'File':
				xmlAsString += SerializeFileElement( child, indentationDepth+1 )
			else:
				raise NameError('Malformed Vc8 project file : unexpected tag in Filter :'+ child.tagName)
	xmlAsString += CreateIndentation( indentationDepth ) + '</Filter>\n'
	return xmlAsString

def SerializeFilesElement( filesElement ):
	if filesElement.tagName != 'Files':
		raise NameError('Malformed Vc8 project file : unexpected tag :'+ filesElement.tagName)
	xmlAsString = CreateIndentation( 1 ) + '<Files>\n'
	for child in filesElement.childNodes:
		if child.nodeType == xml.dom.Node.ELEMENT_NODE:
			if child.tagName == 'Filter':
				xmlAsString += SerializeFileFilterElement( child, 2 )
			elif child.tagName == 'File':
				xmlAsString += SerializeFileElement( child, 2 )
			else:
				raise NameError('Malformed Vc8 project file : unexpected tag in Files :'+ child.tagName)
	xmlAsString += CreateIndentation( 1 ) + '</Files>\n'
	return xmlAsString
	
def SerializeVisualStudioProjectElement( visualStudioProjectElement ):
	xmlAsString = '<VisualStudioProject\n'
	xmlAsString += SerializeAttribute( visualStudioProjectElement, "ProjectType", 1 )
	xmlAsString += SerializeAttribute( visualStudioProjectElement, "Version", 1 )
	xmlAsString += SerializeAttribute( visualStudioProjectElement, "Name", 1 )
	xmlAsString += SerializeAttribute( visualStudioProjectElement, "ProjectGUID", 1 )
	xmlAsString += SerializeAttributeIfExists( visualStudioProjectElement, "RootNamespace", 1 )
	xmlAsString += SerializeAttributeIfExists( visualStudioProjectElement, "Keyword", 1 )
	xmlAsString += SerializeAttribute( visualStudioProjectElement, "TargetFrameworkVersion", 1 )
	xmlAsString += '\t>\n'
	xmlAsString += SerializePlatformsElement( FindNamedChildElement(visualStudioProjectElement, "Platforms") )
	xmlAsString += SerializeToolFilesElement( FindNamedChildElement(visualStudioProjectElement, "ToolFiles") )
	xmlAsString += SerializeConfigurationsElement( FindNamedChildElement(visualStudioProjectElement, "Configurations") )
	xmlAsString += SerializeReferencesElement( FindNamedChildElement(visualStudioProjectElement, "References") )
	xmlAsString += SerializeFilesElement( FindNamedChildElement(visualStudioProjectElement, "Files") )
	xmlAsString += SerializeGenericEmptyElement( FindNamedChildElement(visualStudioProjectElement, "Globals"), "Globals" )
	xmlAsString += '</VisualStudioProject>\n'
	return xmlAsString

def DomToVc9XmlString( dom ):
	'''
		Although vc9 is an xml format and standard xml api could save it, we want to write vc9 file as if it was generated by visual studio,
		and therefore preserve the order of elements, the order of attributes and also the line breaks
		The reason for this is to ease the reading of difference between a version generated by vcproj and a version generated by this tool
		
		@return the serialized string
	'''
	xmlAsString = '<?xml version="1.0" encoding="Windows-1252"?>\n'
	xmlAsString += SerializeVisualStudioProjectElement( dom.documentElement )
	return xmlAsString
	
def Vc8ToVc9( vc8ProjFilePath, vc9ProjFilePath ):
	"""
		converts a visual studio 2005 C project file (a vcproj version 8) into a visual studio 2008 C project file (a vcproj version 9)
		
	"""
	#print( 'converting ' + vc8ProjFilePath + ' to ' + vc9ProjFilePath )
	dom = ParseXmlFile( vc8ProjFilePath )
	#fixEndLines( dom )
	
	# change the version number
	vspNode = dom.documentElement
	if vspNode.tagName != 'VisualStudioProject':
		raise NameError('Malformed Vc8 project file : can\'t find the tag VisualStudioProject')
	
	vspAttrs = vspNode.attributes
	versionAttr = vspAttrs['Version']
	if versionAttr == None:
		raise NameError('Malformed Vc8 project file : can\'t find the tag VisualStudioProject\'s attribute named Version')
	if versionAttr.nodeValue != '8,00' and versionAttr.nodeValue != '8.00':
		raise NameError('Malformed Vc8 project file : expecting Version=8,00, found '+versionAttr.nodeValue)
	versionAttr.nodeValue = '9,00'
	#print('changed Version attribute to '+versionAttr.nodeValue)
	
	# add a new attribute to VisualStudioProject's attributes
	vspAttrs['TargetFrameworkVersion']='131072'
	#print('added TargetFrameworkVersion attribute to VisualStudioProject node')
	
	# delete the VCWebDeployment tool from all configurations
	configurationElements = dom.getElementsByTagName('Configuration')
	for configElement in configurationElements:
		children = configElement.childNodes
		for child in children:
			if child.nodeName == 'Tool':
				if child.attributes['Name'].nodeValue == 'VCWebDeploymentTool':
					configElement.removeChild( child )
					#print('removed VCWebDeploymentTool from configuration '+configElement.attributes['Name'].nodeValue)
					break
		# because of a bug in the removeChild method, we have to remove the empty text node, that otherwise would leave a blank line instead of the removed element
		lastChild = None
		for child in children:
			if lastChild != None:
				if (child.nodeType == xml.dom.Node.TEXT_NODE) and (lastChild.nodeType == xml.dom.Node.TEXT_NODE):
					if len(child.data.strip()) == 0:
						configElement.removeChild( child )
						break
			lastChild = child
	
	# add new attributes to VCLinkerTool
	for configElement in configurationElements:
		children = configElement.childNodes
		for child in children:
			if child.nodeName == 'Tool':
				if child.attributes['Name'].nodeValue == 'VCLinkerTool':
					child.attributes['RandomizedBaseAddress'] = '1'
					child.attributes['DataExecutionPrevention'] = '0'
					#print('added attributes RandomizedBaseAddress and DataExecutionPrevention to VCLinkerTool of configuration '+configElement.attributes['Name'].nodeValue)
					break

	# change VCCLCompilerTool's DebugInformationFormat value 
	# in vc8 :
	#   3 : Base de donnees du programme (/Zi)
	#   4 : Base de donnees du programme pour Modifier & Continuer (/ZI)
	
	'''
	for configElement in configurationElements:
		children = configElement.childNodes
		for child in children:
			if child.nodeName == 'Tool':
				if child.attributes['Name'].nodeValue == 'VCCLCompilerTool':
					debugInfoFormatAttr = child.getAttributeNode('DebugInformationFormat')
					if debugInfoFormatAttr.nodeValue == "0": 
						debugInfoFormatAttr.nodeValue = "3"
					else:
						raise NameError('Unhandled value for DebugInformationFormat : '+debugInfoFormatAttr.nodeValue)
					break
	'''
	
	
	# save the resulting dom
	#vc9AsXmlString = DomToXmlString( dom )
	vc9AsXmlString = DomToVc9XmlString( dom )
	
	# replace _vc8 with _vc9 because some projects output things in diffrent directories depending the compiler version
	vc9AsXmlString = re.sub('_vc8', '_vc9', vc9AsXmlString)
	vc9AsXmlString = re.sub(' vc8', ' vc9', vc9AsXmlString)

	#print(vc9AsXmlString)
	#print('Vc8ToVc9 : writing vc9 file to '+vc9ProjFilePath)
	vc9File = io.open( vc9ProjFilePath, 'wt' )
	vc9File.writelines( vc9AsXmlString )
	vc9File.close()
	
	
def	Vc8ToVc9IfNecessary(vc8ProjFilePath, vc9ProjFilePath):
	tempFilename = tempfile.NamedTemporaryFile().name
	Vc8ToVc9(vc8ProjFilePath, tempFilename)
	overwriteVc9File = True
	if os.path.exists( vc9ProjFilePath ):
		existingVc9File = io.open( vc9ProjFilePath, 'rt' )
		existingVc9AsXmlString = existingVc9File.readlines()
		existingVc9File.close()
		newVc9Dom = ParseXmlFile( tempFilename )
		#newVc9AsXmlString = DomToXmlString(newVc9Dom)
		newVc9AsXmlString = DomToVc9XmlString(newVc9Dom)
		
		existingVc9File = io.open( 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/existingVc9.vcproj', 'wt' )
		existingVc9File.writelines( existingVc9AsXmlString )
		existingVc9File.close()

		newVc9File = io.open( 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/newVc9.vcproj', 'wt' )
		newVc9File.writelines( newVc9AsXmlString )
		newVc9File.close()

		if existingVc9AsXmlString == newVc9AsXmlString:
			print(vc9ProjFilePath + ' won\'t be overwritten because its updated contents would be exactly the same')
			overwriteVc9File = False
	if overwriteVc9File:
		shutil.copyfile(tempFilename, vc9ProjFilePath)
	os.remove( tempFilename )

def Sln2005ToSln2008( vs2005SolutionFilePath, vs2008SolutionFilePath ):
	vs2005SolutionFile = io.open( vs2005SolutionFilePath, 'rt' )
	solutionAsString = vs2005SolutionFile.read(None)
	#print solutionAsString.__class__
	vs2005SolutionFile.close()
	solutionAsString = re.sub('Microsoft Visual Studio Solution File, Format Version 9.00', 'Microsoft Visual Studio Solution File, Format Version 10.00', solutionAsString)
	solutionAsString = re.sub('# Visual Studio 2005', '# Visual Studio 2008', solutionAsString)
	solutionAsString = re.sub('# Visual C\+\+ Express 2005', '# Visual C++ Express 2008', solutionAsString)
	solutionAsString = re.sub('_vc8', '_vc9', solutionAsString)
	solutionAsString = re.sub('msvc8', 'msvc9', solutionAsString)
	#print solutionAsString.__class__
	vs2008SolutionFile = io.open( vs2008SolutionFilePath, 'wt' )
	vs2008SolutionFile.writelines( solutionAsString )
	vs2008SolutionFile.close()


def Vc8ToVc9OnDir( dirPath ):
	for root, dirs, filenames in os.walk(dirPath, topdown=False):
		for filename in filenames:
			isAVc8File = (re.search('_vc8.vcproj$', filename) != None )
			isAVs2005SolutionFile = (re.search('_vc8.sln$', filename) != None )
			isInSubversionDir = ( re.search('\.svn', os.path.join(root, filename)) != None )
			if isAVc8File == True and isInSubversionDir == False:
				vc8Filename = filename
				vc9Filename = re.sub('_vc8.vcproj', '_vc9.vcproj', vc8Filename)
				vc8Filepath = os.path.join(root, vc8Filename)
				vc9Filepath = os.path.join(root, vc9Filename)
				print( 'Vc8ToVc9OnDir : creating ' + vc9Filename + ' from ' + vc8Filename )
				Vc8ToVc9( vc8Filepath, vc9Filepath )
			elif isAVs2005SolutionFile == True and isInSubversionDir == False:
				vs2005SlnFilename = filename
				vs2008SlnFilename = re.sub('_vc8.sln', '_vc9.sln', vs2005SlnFilename)
				vs2005SlnFilepath = os.path.join(root, vs2005SlnFilename)
				vs2008SlnFilepath = os.path.join(root, vs2008SlnFilename)
				print( 'Vc8ToVc9OnDir : creating ' + vs2008SlnFilename + ' from ' + vs2005SlnFilename )
				Sln2005ToSln2008( vs2005SlnFilepath, vs2008SlnFilepath )
	
if __name__ == '__main__':
	Vc8ToVc9OnDir('.')
	#Vc8ToVc9OnDir('C:/dev/solipsis/trunk')
	#Vc8ToVc9IfNecessary( 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/peerPhysX_vc8.vcproj', 'C:/dev/solipsis/trunk/peerPhysics/peerPhysX/peerPhysX_vc9.vcproj' )
	#testFix()
