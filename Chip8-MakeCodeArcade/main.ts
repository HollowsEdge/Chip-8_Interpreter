function Opcode5XY0 (X: number, Y: number) {
    if (registers[X] == registers[Y]) {
        programCounter += 2
    }
}
function OpcodeBNNN (X: number, NNN: number) {
    targetAddress = NNN
    if (jumpQuirk) {
        targetAddress += registers[X]
    } else {
        targetAddress += registers[0]
    }
    programCounter = targetAddress
}
function OpcodeFX33 (X: number) {
    FX33TempValue = registers[X]
    memory[indexRegister + 2] = FX33TempValue % 10
    FX33TempValue = Math.floor(FX33TempValue / 10)
    memory[indexRegister + 1] = FX33TempValue % 10
    FX33TempValue = Math.floor(FX33TempValue / 10)
    memory[indexRegister] = FX33TempValue % 10
}
function Opcode8XY5 (X: number, Y: number) {
    operand1 = registers[X]
    registers[X] = (operand1 - registers[Y]) & 0xFF
    registers[15] = operand1 >= registers[Y] ? 1 : 0
}
function OpcodeFX0A (X: number) {
    FX0AIndex = keyboardReleased.indexOf(true)
    if (FX0AIndex == -1) {
        programCounter += -2
    } else {
        registers[X] = FX0AIndex
    }
}
function Opcode4XNN (X: number, NN: number) {
    if (registers[X] != NN) {
        programCounter += 2
    }
}
function OpcodeEX9E (X: number) {
    if (keyboardPressed[registers[X]]) {
        programCounter += 2
    }
}
function DecToHex (n: number) {
    digits = "0123456789ABCDEF"
    return "" + digits.charAt((n >> 4) & 0xF) + digits.charAt(n & 0xF)
}
function OpcodeDXYN (X: number, Y: number, N: number) {
    displayFlag = true
    displayXValue = registers[X] % screenSizeX
    displayYValue = registers[Y] % screenSizeY
    registers[15] = 0
    for (let y = 0; y <= N - 1; y++) {
        spriteByte = memory[indexRegister + y]
        for (let x = 0; x <= 7; x++) {
            targetXPos = x + displayXValue
            targetYPos = y + displayYValue
            if (targetXPos < screenSizeX && targetYPos < screenSizeY) {
                if ((spriteByte & (0x80 >> x)) != 0) {
                    if (screenImage.getPixel(targetXPos, targetYPos) == 1) {
                        registers[15] = 1
                        screenImage.setPixel(targetXPos, targetYPos, 15)
                    } else {
                        screenImage.setPixel(targetXPos, targetYPos, 1)
                    }
                }
            }
        }
    }
}
function OpcodeFX15 (X: number) {
    delayTimer = registers[X]
}
function OpcodeFX18 (X: number) {
    soundTimer = registers[X]
}
function OpcodeFX55 (X: number) {
    for (let index2 = 0; index2 <= X; index2++) {
        memory[indexRegister + index2] = registers[index2]
    }
    if (memoryQuirk) {
        indexRegister += X + 1
    }
}
function ExecuteInstruction () {
    currentInstruction = (memory[programCounter] << 8) | memory[programCounter + 1]
programCounter += 2
    registerXNum = (currentInstruction >> 8) & 0xF
registerYNum = (currentInstruction >> 4) & 0xF
let firstNibble = (currentInstruction >> 12) & 0xF
NNN = currentInstruction & 0xFFF
NN = currentInstruction & 0xFF
N = currentInstruction & 0xF
if (firstNibble == 0) {
        if (N == 0) {
            Opcode00E0()
        } else if (N == 14) {
            Opcode00EE()
        }
    } else if (firstNibble == 1) {
        Opcode1NNN(NNN)
    } else if (firstNibble == 2) {
        Opcode2NNN(NNN)
    } else if (firstNibble == 3) {
        Opcode3XNN(registerXNum, NN)
    } else if (firstNibble == 4) {
        Opcode4XNN(registerXNum, NN)
    } else if (firstNibble == 5) {
        Opcode5XY0(registerXNum, registerYNum)
    } else if (firstNibble == 6) {
        Opcode6XNN(registerXNum, NN)
    } else if (firstNibble == 7) {
        Opcode7XNN(registerXNum, NN)
    } else if (firstNibble == 8) {
        if (N == 0) {
            Opcode8XY0(registerXNum, registerYNum)
        } else if (N == 1) {
            Opcode8XY1(registerXNum, registerYNum)
        } else if (N == 2) {
            Opcode8XY2(registerXNum, registerYNum)
        } else if (N == 3) {
            Opcode8XY3(registerXNum, registerYNum)
        } else if (N == 4) {
            Opcode8XY4(registerXNum, registerYNum)
        } else if (N == 5) {
            Opcode8XY5(registerXNum, registerYNum)
        } else if (N == 6) {
            Opcode8XY6(registerXNum, registerYNum)
        } else if (N == 7) {
            Opcode8XY7(registerXNum, registerYNum)
        } else if (N == 14) {
            Opcode8XYE(registerXNum, registerYNum)
        }
    } else if (firstNibble == 9) {
        Opcode9XY0(registerXNum, registerYNum)
    } else if (firstNibble == 10) {
        OpcodeANNN(NNN)
    } else if (firstNibble == 11) {
        OpcodeBNNN(registerXNum, NNN)
    } else if (firstNibble == 12) {
        OpcodeCXNN(registerXNum, NN)
    } else if (firstNibble == 13) {
        OpcodeDXYN(registerXNum, registerYNum, N)
    } else if (firstNibble == 14) {
        if (NN == 158) {
            OpcodeEX9E(registerXNum)
        } else if (NN == 161) {
            OpcodeEXA1(registerXNum)
        }
    } else if (firstNibble == 15) {
        if (NN == 7) {
            OpcodeFX07(registerXNum)
        } else if (NN == 10) {
            OpcodeFX0A(registerXNum)
        } else if (NN == 21) {
            OpcodeFX15(registerXNum)
        } else if (NN == 24) {
            OpcodeFX18(registerXNum)
        } else if (NN == 30) {
            OpcodeFX1E(registerXNum)
        } else if (NN == 41) {
            OpcodeFX29(registerXNum)
        } else if (NN == 51) {
            OpcodeFX33(registerXNum)
        } else if (NN == 85) {
            OpcodeFX55(registerXNum)
        } else if (NN == 101) {
            OpcodeFX65(registerXNum)
        }
    }
    if (displayFlag) {
        screenSprite.setImage(screenImage)
        displayFlag = false
    }
}
function OpcodeANNN (NNN: number) {
    indexRegister = NNN
}
function OpcodeFX65 (X: number) {
    for (let index = 0; index <= X; index++) {
        registers[index] = memory[indexRegister + index]
    }
    if (memoryQuirk) {
        indexRegister += X + 1
    }
}
function Opcode00EE () {
    programCounter = programStack.pop()
}
function Opcode8XY4 (X: number, Y: number) {
    totalValue = registers[X] + registers[Y]
    registers[X] = totalValue & 0xFF
    registers[15] = totalValue > 255 ? 1 : 0
}
function OpcodeEXA1 (X: number) {
    if (!(keyboardPressed[registers[X]])) {
        programCounter += 2
    }
}
function Opcode8XY7 (X: number, Y: number) {
    operand12 = registers[X]
    registers[X] = (registers[Y] - operand12) & 0xFF
    registers[15] = registers[Y] >= operand12 ? 1 : 0
}
function OpcodeFX1E (X: number) {
    indexRegister += registers[X]
    if (indexQuirk) {
        if (indexRegister > 4095) {
            registers[15] = 1
        }
    }
}
function Opcode1NNN (NNN: number) {
    programCounter = NNN
}
function OpcodeCXNN (X: number, NN: number) {
    registers[X] = (randint(0, 255) & NN) & 0xFF
}
function Opcode8XY6 (X: number, Y: number) {
    if (shiftQuirk) {
        operand13 = registers[X]
        registers[X] = (operand13 >> 1) & 0xFF
        registers[15] = operand13 & 1
    } else {
        registers[X] = (registers[Y] >> 1) & 0xFF
        registers[15] = registers[Y] & 1
    }
}
function OpcodeFX07 (X: number) {
    registers[X] = delayTimer
}
function Opcode3XNN (X: number, NN: number) {
    if (registers[X] == NN) {
        programCounter += 2
    }
}
function Opcode2NNN (NNN: number) {
    programStack.push(programCounter)
    programCounter = NNN
}
function Opcode6XNN (X: number, NN: number) {
    registers[X] = NN
}
function Opcode8XY2 (X: number, Y: number) {
    registers[X] = registers[X] & registers[Y]
}
function Opcode8XY1 (X: number, Y: number) {
    registers[X] = registers[X] | registers[Y]
}
function Opcode8XY0 (X: number, Y: number) {
    registers[X] = registers[Y]
}
function Opcode8XY3 (X: number, Y: number) {
    registers[X] = registers[X] ^ registers[Y]
}
function OpcodeFX29 (X: number) {
    indexRegister = 80 + registers[X] * 5
}
function Opcode7XNN (X: number, NN: number) {
    registers[X] = (registers[X] + NN) & 0xFF
}
function Opcode00E0 () {
    displayFlag = true
    screenImage.fill(15)
}
function Opcode8XYE (X: number, Y: number) {
    if (shiftQuirk) {
        operand14 = registers[X]
        registers[X] = (registers[X] << 1) & 0xFF
        registers[15] = (operand14 >> 7) & 1
    } else {
        registers[X] = (registers[Y] << 1) & 0xFF
        registers[15] = (registers[Y] >> 7) & 1
    }
}
function Opcode9XY0 (X: number, Y: number) {
    if (registers[X] != registers[Y]) {
        programCounter += 2
    }
}
let timerAccumulator = 0
let cpuAccumulator = 0
let dt = 0
let now = 0
let soundTimer = 0
let delayTimer = 0
let targetYPos = 0
let targetXPos = 0
let displayYValue = 0
let displayXValue = 0
let displayFlag = false
let digits = ""
let FX0AIndex = 0
let FX33TempValue = 0
let targetAddress = 0
let previousKeyboardPressed: boolean[] = []
let keyboardPressed: boolean[] = []
let keyboardReleased: boolean[] = []
let indexRegister = 0
let programStack: number[] = []
let screenSprite: Sprite = null
let screenImage: Image = null
let memoryQuirk = false
let indexQuirk = false
let jumpQuirk = false
let shiftQuirk = false
let screenSizeY = 0
let screenSizeX = 0
let operand14 = 0
let operand13 = 0
let operand12 = 0
let totalValue = 0
let operand1 = 0
let index3 = 0
let binVals: number[] = []
let binaryString2 = ""
let hexChars2 = ""
let programCounter = 0
let ROMData = ""
let targetByteSpriteData = ""
let decimalNumber = 0
let binaryString = ""
let registers: number[] = []
let N = 0
let NN = 0
let NNN = 0
let registerYNum = 0
let registerXNum = 0
let currentInstruction = 0
let memory: number[] = []
let hexString = ""
let remainder = 0
let hexChars = ""
let tempValue = 0
let remainder2 = 0
let spriteByte = 0
scene.setBackgroundColor(12)
let currentlyPressedKeys = [
browserEvents.X.isPressed(),
browserEvents.One.isPressed(),
browserEvents.Two.isPressed(),
browserEvents.Three.isPressed(),
browserEvents.Q.isPressed(),
browserEvents.W.isPressed(),
browserEvents.E.isPressed(),
browserEvents.A.isPressed(),
browserEvents.S.isPressed(),
browserEvents.D.isPressed(),
browserEvents.Z.isPressed(),
browserEvents.C.isPressed(),
browserEvents.Four.isPressed(),
browserEvents.R.isPressed(),
browserEvents.F.isPressed(),
browserEvents.V.isPressed()
]
screenSizeX = 64
screenSizeY = 32
shiftQuirk = true
jumpQuirk = true
indexQuirk = true
memoryQuirk = true
screenImage = image.create(screenSizeX, screenSizeY)
screenImage.fill(15)
screenSprite = sprites.create(screenImage, SpriteKind.Player)
screenSprite.setScale(2.5, ScaleAnchor.Middle)
programStack = []
programCounter = 512
indexRegister = 0
let timerSpeed = 1 / 60 * 1000
let instructionCycles = 10
for (let index = 0; index < 4096; index++) {
    memory.push(0)
}
keyboardReleased = []
for (let index = 0; index < 16; index++) {
    keyboardReleased.push(false)
}
for (let index = 0; index < 16; index++) {
    keyboardPressed.push(false)
}
for (let index = 0; index < 16; index++) {
    previousKeyboardPressed.push(false)
}
registers = []
for (let index = 0; index < 16; index++) {
    registers.push(0)
}
let fontData = "F0909090F02060202070F010F080F0F010F010F09090F01010F080F010F0F080F090F0F010204040F090F090F0F090F010F0F090F09090E090E090E0F0808080F0E0909090E0F080F080F0F080F08080"
for (let index42 = 0; index42 <= fontData.length / 2 - 1; index42++) {
    memory[80 + index42] = parseInt("0x" + ROMData.substr(index42 * 2, 2), 16)
}
game.showLongText("ROM Selection : 0 - Outlaw | 1 - Br8kout | 2 - Glitch Ghost | 3 - Snek", DialogLayout.Full)
let userROMSelection = game.askForNumber("Select a ROM", 1)
if (userROMSelection == 0) {
    ROMData = "13b438383b1b1b1bdfded8d8f878181c0c000070f870677c60607828ec183e1c187e9999995a3c66c3183e1c187e9999995a3c24360000061f0ee63e06061e1437187c38187e9999995a3c66c3187c38187e9999995a3c246cffff806006e09e00ee69016005e0a169026008e0a169038be07b098ad07a05a25bdba100eedba17b0149027aff49037a014a0169034a1e69024b3f6900490000eedba14f0000ee690000ee330000ee6301c0034001630240026303858075ff84707405a25bd54100eed54175ff430274ff4303740144016303441e630245006300430000eed5414f0000ee630000ee81e082d06007e0a171ff6009e0a171016005e0a172ff6008e0a17201600051e0601852d060184c18600c6f06efa1600041006101411561144200620142126211a211fc1ededc8e108d208c00a211fc1ededc00ee71ff00ee710100ee72ff00ee72ff00ee720100ee720100ee800000ee22a400eec01cb33c81808270235c600051806018527060184618600c41236124413861374200620142126211a235f61ed87c881087208600a235f61ed87c00eed87ca25bdba16020f018f015f007300013a413b4dedca25bd541139e6e056d0a6c006833670a66006900630000e0a202601c6109d01fa2596000611fd0127008304013d4a211dedca235d87c22e83f0013ac23603f001398a25b3900227e4900225c330022c213e4"
} else if (userROMSelection == 1) {
    ROMData = "129ffcfc80a202ddc100eea204dba100eea2036002610587008610d67171086f388f174f00121770026f108f074f00121500ee22057d04220500ee22057dfc220500ee8080400168ff40ff68015ac0225300ee80b070fb61f880127005a203d0a100ee220b8b948a84220b4b0069014b3f69ff4a0068014a1f68ff4f0122434a1f228500ee00e06b1e6a142205220b221100eefe073e0012936e04fe1500ee6d1e6c1e6b406a1dc901490069ff68ff2205220b22116007e0a1223b6009e0a122332263229312b5"
} else if (userROMSelection == 2) {
    ROMData = "1cf53c62c0c0ded2c2663c00000000c04040404080447c00e040406040804040e0000000bef216101010101030383c62c0c0c0c0c2663c0000000084c244447c4646844000003c62c0c0ded2c2663c0084c244447c46468440007cc6c282868484cc78000000007cc4c07808c2a48c78bef21610101010103038000000000000f090f09090e0a0f090f0f0808080f0e0a09090f0f080e080f0f080e08080e080b090f09090f09090e0404040e060202020e090a0c0a09080808080f090f0b0909090d0d0b090f0909090f0f090f08080e09090b070e090f0a090f080f010f0f04040404090909090f09090a0c0809090d0f09090d060b0909090e04040f03060c0f00000000040808080008000007c547cfe7c54000038007c54fe7c7c54000000107cd67c7c7c540000000010e0e04040e0a0a0f8a8f8f8f8a80000002000f8a8f8f8f8a8000070f8e8f8f8f8a80000002000f8e8f8f8f8a8000070f8b8f8f8f8a80000002000f8b8f8f8f8a8000070f8f8f8f8f8a80000002000f8f8f8f8f8a80000700060606000000000000040a0a0e000000000000040e040400000000000000060f0900000000000000060f0900090009000900060f09000f060f060f040c04002030240e0e0409010d412105600000b00000000001f0d00000000000000000000000000000000000000000000350c00000000000000000000000000000000000000000000131200000000000000000000000000000000000000000000320400000000000000000000000000000000000000000000320d00000000000000000000000000000000000000000000180e000000000000000000000000000000000000000000000305000000000000000000000000000000000000000000000a010000000000000000000000000000000000000000000009160000000000000000000000000000000000000000000002070900170900270901370903071602171601271603371602130b00190b02200b00250b022c0b03031603351904070803090303190300290501071402210e01201703291904340c030400003a0503360c011818012518021b0e02220e041e12021204022204023204013707011212002016032a1204070903040a003202010d01031f1b02021502230e023219043711030300000703033500003908013319021412021418041c1100390303041f031d1302130a02131002280a022810041d09010d18030801001b0302150e023312012a05023a120436060000000000000000000001320c000000000000000000042e15000000006a0f6b0d6c006d0000eea334fc1edaba00ee610aa5a4f065801482f0a5a4f0554201265d00ee81a082b06007e0a171ff6009e0a171016005e0a172ff6008e0a17201600051a0600152b06001400000ee600139006000320160004001620260013990600032156000400162146001380060003101600040016102600138306000313a60004001613963008fa08f154f0063148fa08f174f0063288fb08f274f00633c83d4a334fc1edabaa334f31ed12a8a108b208c3000ee81d082c041007c0a41006d0a410a7cf6410a6d00a334f21edabaa334fc1edaba00eea334fc1edaba610060a0a334f01e2ce9f11edaba710a3196168d00e049d82bcd28a325afa5a2f165580000ee591000ee26b985002771279700ee82a0720383607302600092306000923064008f308f254f0060038f308f254f0084208f308f254f0084358f308f274f0060048f308f274f0084308f308f274f00842582b0720483707303610092306100923065008f308f254f0061018f308f254f0085208f308f254f0085358f308f274f0061028f308f274f0085308f308f274f00852562008f408f573f0082108f408f574f00820080204000c20340007201802000ee661f670e6000a5a1f05560006100a5a2f15500ee8e008050a5a1f05580e000ee8e00a5a1f065850080e000eea3d8f81ef91ef1658600871000eea5a2f165580000ee591000eea32dd677277d350027af00ee277d2ce925cb25b7a3d38050277164004a3c293b4aff29514bff29674b17297d4401650044012771440100eea3d3277d850081608270450172ff45027201450371ff45047101a32dd677d12784f0440086104400872044012993400a00ee44012847600046006001463d60014700600147196001400017b1d677a5a2f165460070e8463d7018470071b847197148a5a2f1556500277100ee277d8050400165044003650140026503400465022771a32dd127d67700ee275d00e028a325af279700eea38400eea38e00eea39800eea3a200eea3ac00eea3b600ee00eeb87100eea4b0f41ef81ef91ef265800e800e288bd12a00ee480028bf483028d9490028f3499029116400288f7403341818b500ee640165214990640049906520a3c06100d1437404545018cf00ee640165214990640049906520a3c36138d1437404545018e900ee640165414830640048306540a3c66100d4127404545019034800292b00ee640165414830640048306540a3c8611ed41274045450192100ee60156100a3cad0137008a3cdd01300ee78186a006c146d00278900e028a3279725af640100ee78e86a3b6c286d00278900e028a3279725af640100ee79b86b166c3c6d00278900e028a3279725af640100ee79486b006c006d00278900e028a3279725af640100eea3d3f2556015a4b0f81ef91ef01ef265850083108420a3d3f265350419e96001853075fe8f508f174f00600075058f508f154f006000854075fa8f508f274f006000750f8f508f254f006000400019e929ef400a00ee6401600000ee60056515a4b0f81ef91ef51ef0552a0b00e0286549d82bcd600a00eea3d0f0657001a3d0f05500ee6100620ce2a1610141012a0b60024101bcf56206e29e1a172ce92ce900ee00e02a492a172a932a172b592a172c1b00e000ee630b61086204a202d12b7108f31ed12b7107f31ed12b7104f31ed12b7108f31ed12b7108f31ed12b61106210f31ed12b7108f31ed12b7108f31ed12b7108f31ed12b7107f31ed12b00ee00e06102620160072bfb600c2bfb60092bfb60142bfb60032bfb60082bfb60002bfb60072bfb60082bfb600f2bfb60132bfb60142bfb2c1560092bfb60132bfb60002bfb60142bfb60082bfb60052bfb60002bfb600f2bfb600e2bfb600c2bfb60192bfb2c1560072bfb60082bfb600f2bfb60132bfb60142bfb60002bfb60092bfb600e2bfb60002bfb60142bfb60082bfb60052bfb2c1560032bfb60052bfb600d2bfb60052bfb60142bfb60052bfb60122bfb60192bfb601b2bfb601b2bfb601b2bfb00ee00e06102620160082bfb60052bfb600c2bfb60102bfb60002bfb60142bfb60082bfb60052bfb600d2bfb2c15600d2bfb60012bfb600b2bfb60052bfb60002bfb60132bfb600f2bfb600d2bfb60052bfb2c1560062bfb60122bfb60092bfb60052bfb600e2bfb60042bfb60132bfb601c2bfb00ee6108620460052bfb60002bfb60142bfb600f2bfb60002bfb60082bfb60012bfb60152bfb600e2bfb60142bfb00ee40001c11640065007505740154001c03a27bf51ed125710500ee6102720600ee00e0680169d825a5275d600161d8a5a2f1552bcd28a325af279764002ce9740134281c37267f64002ce9740134281c4300ee00e02c616f0aff15ff073f001c552cc11c5100eea3d1f0656102620fa30cf01ed12b61076202a30cf01ed12b610f6209a30cf01ed12b61186204a30cf01ed12b611d6211a30cf01ed12b61226204a30cf01ed12b612b6209a30cf01ed12b61336202a30cf01ed12b6138620fa30cf01ed12b00ee2c61a3d2f0658100a3d1f06582008214421661f54200610ba3d28010f055a3d18020f0552c6100eeff073f001ce96f02ff1500ee2a3500e06800690025a525af275d279728a32ce9a3d0f06540092d2f25cb6006e0a1267f4a3c293b4aff29514bff29674b17297d25b71d0700ee61006220801063002ce99010f2189020f118840094108020942080107301330a1d376000a3d0f0552c4d00ee"
} else if (userROMSelection == 3) {
    ROMData = "a238f76500e0ff073f001206f315e4a16200e5a16201e6a16202e7a1620342007001420171ff420270ff42037101d0114f001206ff0a1200201000010905070880"
} else {
    game.setGameOverPlayable(false, music.melodyPlayable(music.jumpDown), false)
    game.setGameOverEffect(false, effects.none)
    game.setGameOverMessage(false, "Invalid Selection")
    game.gameOver(false)
}
for (let index422 = 0; index422 <= ROMData.length / 2 - 1; index422++) {
    memory[512 + index422] = parseInt("0x" + ROMData.substr(index422 * 2, 2), 16)
}
let cpuSpeed = game.askForNumber("Set Speed (default 500) : ", 4)
if (cpuSpeed < 0) {
    game.setGameOverPlayable(false, music.melodyPlayable(music.jumpDown), false)
    game.setGameOverEffect(false, effects.none)
    game.setGameOverMessage(false, "Invalid Selection")
}
let lastTime = game.runtime()
game.onUpdate(function () {
    now = game.runtime()
    dt = now - lastTime
    lastTime = now
    cpuAccumulator += dt * cpuSpeed / 1000
    timerAccumulator += dt
    while (cpuAccumulator >= 1) {
        cpuAccumulator += 0 - 1
        ExecuteInstruction()
    }
    while (timerAccumulator >= 1000 / 60) {
        timerAccumulator += 0 - 1000 / 60
        if (delayTimer > 0) {
            delayTimer += -1
        }
        if (soundTimer > 0) {
            soundTimer += -1
        }
    }
})
forever(function () {
    currentlyPressedKeys[0] = browserEvents.X.isPressed()
    currentlyPressedKeys[1] = browserEvents.One.isPressed()
    currentlyPressedKeys[2] = browserEvents.Two.isPressed()
    currentlyPressedKeys[3] = browserEvents.Three.isPressed()
    currentlyPressedKeys[4] = browserEvents.Q.isPressed()
    currentlyPressedKeys[5] = browserEvents.W.isPressed()
    currentlyPressedKeys[6] = browserEvents.E.isPressed()
    currentlyPressedKeys[7] = browserEvents.A.isPressed()
    currentlyPressedKeys[8] = browserEvents.S.isPressed()
    currentlyPressedKeys[9] = browserEvents.D.isPressed()
    currentlyPressedKeys[10] = browserEvents.Z.isPressed()
    currentlyPressedKeys[11] = browserEvents.C.isPressed()
    currentlyPressedKeys[12] = browserEvents.Four.isPressed()
    currentlyPressedKeys[13] = browserEvents.R.isPressed()
    currentlyPressedKeys[14] = browserEvents.F.isPressed()
    currentlyPressedKeys[15] = browserEvents.V.isPressed()
    for (let k = 0; k <= 15; k++) {
        keyboardReleased[k] = previousKeyboardPressed[k] && !(currentlyPressedKeys[k])
        keyboardPressed[k] = currentlyPressedKeys[k]
        previousKeyboardPressed[k] = currentlyPressedKeys[k]
    }
})
