import os, sys, string

names = {"Int": "int", "Float":"float", "String":"string", "Form":"Form"}

fileNames = ["Array.psc", "Set.psc"]

outputFolder = "../DS/"

for fileName in fileNames:
    content = ""
    with open(fileName, 'r') as reader:
        content = reader.read()
    for hiName, loName in names.items():
        outFileName = hiName+fileName
        with open(outputFolder+outFileName, "w") as writer:
            result = content.format(hiName=hiName, loName=loName)
            writer.write(result)
            print (f"    <Script>DS\{outFileName}</Script>")


valueDictFileName = "ValueDict.psc"
keyTypes = {"Int": "int", "String":"string", "Form":"Form"}
valueTypes = {"Int": "int", "Float":"float", "String":"string", "Form":"Form"}
content = ""
with open(valueDictFileName, 'r') as reader:
    content = reader.read()
    for keyTypeName, keyType in keyTypes.items():
        for valueTypeName, valueType in valueTypes.items():
            outFileName = keyTypeName+"Dict"+valueTypeName+".psc"
            with open(outputFolder+outFileName, "w") as writer:
                result = content.format(keyType=keyType, valueType=valueType, keyTypeName=keyTypeName, valueTypeName=valueTypeName)
                writer.write(result)
                print (f"    <Script>DS\{outFileName}</Script>")


structValueDictFileName = "StructValueDict.psc"
keyTypes = {"Int": "int", "String":"string", "Form":"Form"}
content = ""
with open(structValueDictFileName, 'r') as reader:
    content = reader.read()
    for keyTypeName, keyType in keyTypes.items():
            outFileName = keyTypeName+"DictStruct.psc"
            with open(outputFolder+outFileName, "w") as writer:
                result = content.format(keyType=keyType, keyTypeName=keyTypeName)
                writer.write(result)
                print (f"    <Script>DS\{outFileName}</Script>")

arrayDictFileName = "ArrayDict.psc"
keyTypes = {"Int": "int", "String":"string", "Form":"Form"}
valueTypes = {"Int": "int", "Float":"float", "String":"string", "Form":"Form"}
content = ""
with open(arrayDictFileName, 'r') as reader:
    content = reader.read()
    for keyTypeName, keyType in keyTypes.items():
        for valueTypeName, valueType in valueTypes.items():
            outFileName = keyTypeName+"Dict"+valueTypeName+"Array.psc"
            with open(outputFolder+outFileName, "w") as writer:
                result = content.format(keyType=keyType, valueType=valueType, keyTypeName=keyTypeName, valueTypeName=valueTypeName)
                writer.write(result)
                print (f"    <Script>DS\{outFileName}</Script>")

structArrayDictFileName = "StructArrayDict.psc"
keyTypes = {"Int": "int", "String":"string", "Form":"Form"}
content = ""
with open(structArrayDictFileName, 'r') as reader:
    content = reader.read()
    for keyTypeName, keyType in keyTypes.items():
            outFileName = keyTypeName+"DictStructArray.psc"
            with open(outputFolder+outFileName, "w") as writer:
                result = content.format(keyType=keyType, keyTypeName=keyTypeName)
                writer.write(result)
                print (f"    <Script>DS\{outFileName}</Script>")

