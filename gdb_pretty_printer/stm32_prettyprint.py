
# python import sys;sys.path.append("/home/mmccoo/share");import stm32_prettyprint;stm32_prettyprint.register_printers(None)

# test
#  python print(stm32_prettyprint.stm32_lookup_function(gdb.parse_and_eval("LED_GPIO_Port")))



# based largely on this webpage:
# https://www.rethinkdb.com/blog/make-debugging-easier-with-custom-pretty-printers/
# and this one http://tromey.com/blog/?p=524
import gdb
import gdb.printing
import re
import pdb
#  pdb.set_trace()

def tuple_for_range(rng):
    if (type(rng) is int):
        rng = (rng, rng)
        
    if (type(rng) is str):
        l,h = re.split('[:,. ]', rng)
        rng = (int(l), int(h))

    if ((type(rng) is tuple) & (rng[0] > rng[1])):
        rng = (rng[1], rng[0])

    return rng

def invokePrintersOnFields(val, fields):
    children = []
    for fieldprinter in fields:
        children.append(fieldprinter.to_string(val))

    return "{" + ", ".join(children) + "}"
    
class NumField:
    def __init__(self, name, rng):
        self.name = name
        self.rng = tuple_for_range(rng)
        self.mask = 0;

        for i in range(self.rng[0], self.rng[1]+1):
            self.mask = self.mask | (1<<i)
            
    def to_string(self, value):
        value = int(value)
        masked = (value&self.mask) >> self.rng[0]
        return "{} = {}".format(self.name, str(masked))
        
class Field:
    def __init__(self, name, rng, vals):
        self.name  = name

        self.rng = tuple_for_range(rng)

        self.mask = 0;
        for i in range(self.rng[0], self.rng[1]+1):
            self.mask = self.mask | (1<<i)
            
        numbits = self.rng[1]-self.rng[0]+1

        self.vals = {}
        for val in vals:
            k,v = val
            if (type(k) is int):
                self.vals[k] = v
                continue

            if (type(k) is tuple) | (type(k) is list):
                for i in k:
                    self.vals[i] = v
                continue

            if (type(k) is not str):
                raise ValueError("expecting strings got " . str(k))

            mo = re.match('^0[bB]([01]+)$', k)
            if (mo):
                self.vals[int(mo.group(1), 2)]
                continue

            mo = re.match('^0[xX]([01]+)$', k)
            if (mo):
                self.vals[int(mo.group(1), 16)]
                continue

            mo = re.match('^0[bB]([01xX]+)$', k)
            if (mo):
                # values like 0b0xx
                bits = mo.group(1)
                cares = re.sub("[01]", "1", bits)
                cares = re.sub("[xX]", "0", cares)
                cares = int(cares,2)
                for i in range(1<<numbits):
                    if (i & cares) == cares:
                        self.vals[i] = v


    def to_string(self, value):
        value = int(value)
        masked = (value&self.mask) >> self.rng[0]

        retval = None
        if (masked in self.vals):
            retval = '"' + self.vals[masked] + '"'
        else:
            retval =  "No Val"
        return "{} = {}".format(self.name, retval)   
                
RCC_CR = [
    Field("PLLRDY", 25, [
        (0, "PLL unlocked"),
        (1, "PLL locked")]),
    Field("PLLON", 24, [
        (0, "PLL OFF"),
        (1, "PLL ON")]),
    Field("CSSON", 19, [
        (0, "Clock detector OFF"),
        (1, "Clock detector ON (if HSE is ready)")]),
    Field("HSEBYP", 18, [
        (0, "External 4-16 MHz osc is not bypassed"),
        (1, "External 4-16 MHz osc is bypassed")]),
    Field("HSERDY", 17, [
        (0, "Osc not ready"),
        (1, "Osc ready")]),
    Field("HSEON", 16, [
        (0, "HSE Osc OFF"),
        (1, "HSE Osc ON")]),
    NumField("HSICAL", "15:8"),
    NumField("HSITRIM", "7:3"),
    Field("HSIRDY", 1, [
        (0, "Internal 8MHz RC Osc not ready"),
        (1, "Internal 8MHz RC Osc ready")]),
    Field("HSION", 0, [
        (0, "Internal 8MHz RC Osc OFF"),
        (1, "Internal 8MHz RC Osc ON")])
]

RCC_CFGR = [
    Field("MCO", "26:24", [
        ("0b0xx", "No Clock"),
        (0b100, "System Clock (SYSCLK) selected"),
        (0b101, "HSI clock selected"),
        (0b110, "HSE clock selected"),
        (0b111, "PLL clock divided by 2 selected")
    ]),
    Field("USBPRE", 22, [
        (0, "PLL clock is divided by 1.5"),
        (1, "PLL clock is not divided")
    ]),
    Field("PLLMUL", "21:18" , [
        (0x0000, "PLL input clock x 2"),
        (0x0001, "PLL input clock x 3"),
        (0x0010, "PLL input clock x 4"),
        (0x0011, "PLL input clock x 5"),
        (0x0100, "PLL input clock x 6"),
        (0x0101, "PLL input clock x 7"),
        (0x0110, "PLL input clock x 8"),
        (0x0111, "PLL input clock x 9"),
        (0x1000, "PLL input clock x 10"),
        (0x1001, "PLL input clock x 11"),
        (0x1010, "PLL input clock x 12"),
        (0x1011, "PLL input clock x 13"),
        (0x1100, "PLL input clock x 14"),
        (0x1101, "PLL input clock x 15"),
        (0x1110, "PLL input clock x 16"),
        (0x1111, "PLL input clock x 16")
    ]),
    Field("PLLXTPRE", 17, [
        (0, "HSE clock not divided"),
        (1, "HSE clock divided by 2")
    ]),
    Field("PLLSRC", 16, [
        (0, "HSI Osc clock/2 selected as PLL input"),
        (1, "HSE Osc clock selected as PLL input")
    ]),
    Field("ADCPRE", "15:14", [
        (0b00, "PCLK2 divided by 2"),
        (0b01, "PCLK2 divided by 4"),
        (0b10, "PCLK2 divided by 6"),
        (0b11, "PCLK2 divided by 8")
    ]),
    Field("PPRE2", "13:11", [
        ('0b0xx', "HCLK not divided"),
        (0b100, "HCLK divided by 2"),
        (0b101, "HCLK divided by 4"),
        (0b110, "HCLK divided by 8"),
        (0b111, "HCLK divided by 16")
    ]),
    Field("PPRE1", "10:8", [
        ('0bx00', "HCLK not divided"),
        (0b100, "HCLK divided by 2"),
        (0b101, "HCLK divided by 4"),
        (0b110, "HCLK divided by 8"),
        (0b111, "HCLK divided by 16")
    ]),
    Field("HPRE", "7:4", [
        ('0b0xxx', "SYSCLK not divided"),
        (0b1000, "SYSCLK divided by 2"),
        (0b1001, "SYSCLK divided by 4"),
        (0b1010, "SYSCLK divided by 8"),
        (0b1011, "SYSCLK divided by 16"),
        (0b1100, "SYSCLK divided by 64"),
        (0b1101, "SYSCLK divided by 128"),
        (0b1110, "SYSCLK divided by 256"),
        (0b1111, "SYSCLK divided by 512")
    ]),
    Field("SWS", "3:2" , [
        (0b00, "HSI osc used as system clock"),
        (0b01, "HSE osc used as system clock"),
        (0b10, "PLL used as system clock"),
        (0b11, "not applicable")
    ]),
    Field("SW", "1:0", [
        (0b0, "HSI selected as system clock"),
        (0b1, "HSE selected as system clock"),
        (0b0, "PLL selected as system clock"),
        (0b1, "not allowed")
    ]),
    
]

RCC_CIR = [
    # fields 13-31 are either reserved or write only. No
    # point in printing them
    Field("PLLRDYIE", 12, [
        (0, "PLL lock interrupt disabled"),
        (1, "PLL lock interrupt enabled")
    ]),
    Field("HSERDYIE", 11, [
        (0, "HSE ready interrupt disabled"),
        (1, "HSE ready interrupt enabled")
    ]),
    Field("HSIRDYIE", 10, [
        (0, "HSI ready interrupt disabled"),
        (1, "HSI ready interrupt enabled")
    ]),
    Field("LSERDYIE", 9, [
        (0, "LSE ready interrupt disabled"),
        (1, "LSE ready interrupt enabled")
    ]),
    Field("LSIRDYIE", 8, [
        (0, "LSI ready interrupt disabled"),
        (1, "LSI ready interrupt enabled")
    ]),
    Field("CSSF", 7, [
        (0, "No clock security interrupt caused by HSE clock failure"),
        (1, "Clock security interrupt caused by HSE clock failure")
    ]),
    Field("PLLRDYF", 4, [
        (0, "No clock ready interrupt caused by PLL lock"),
        (1, "Clock interrupt caused by PLL lock")
    ]),
    Field("HSERDYF", 3, [
        (0, "No clock ready interrupt caused by external 4-16MHz oscillator"),
        (1, "Clock ready interrupt caused by external 4-16MHz oscillator")
    ]),
    Field("HSIRDYF", 2, [
        (0, "No clock ready interrupt caused by internal 8MHz oscillator"),
        (1, "Clock ready interrupt caused by internal 8MHz oscillator")
    ]),
    Field("LSERDYF", 1, [
        (0, "No clock ready interrupt caused by external 32kHz"),
        (1, "Clock ready interrup caused by external 32kHz")
    ]),
    Field("LSIRDYF", 0, [
        (0, "No clock ready interrupt caused by internal RC 40kHz"),
        (1, "Clock ready interrupt caused by internal RC 40kHz")
    ])
]

RCC_APB2RSTR = [
    # bits 22-31 are reserved
    Field("TIM11RST", 21, [
        (0, "No effect"),
        (1, "Reset TIM11 timer")
    ]),
    Field("TIM10RST", 20, [
        (0, "No effect"),
        (1, "Reset TIM10 timer")
    ]),
    Field("TIM9RST", 19, [
        (0, "No effect"),
        (1, "Reset TIM9 timer")
    ]),
    # 16-18 reserved
    Field("ADC3RST", 15, [
        (0, "No effect"),
        (1, "Reset ADC3 interface")
    ]),
    Field("USART1RST", 14, [
        (0, "No effect"),
        (1, "Reset USART1")
    ]),
    Field("TIM8RST", 13, [
        (0, "No effect"),
        (1, "Reset TIM8 timer")
    ]),
    Field("SPI1RST", 12, [
        (0, "No effect"),
        (1, "Reset SPI1")
    ]),
    Field("TIM1RST", 11, [
        (0, "No effect"),
        (1, "Reset TIM1 timer")
    ]),
    Field("ADC2RST", 10, [
        (0, "No effect"),
        (1, "Reset ADC2 interface")
    ]),
    Field("ADC1RST", 9, [
        (0, "No effect"),
        (1, "Reset ADC1 interface")
    ]),
    Field("IOPGRST", 8, [
        (0, "No effect"),
        (1, "Reset IO port G")
    ]),
    Field("IOPFRST", 7, [
        (0, "No effect"),
        (1, "Reset IO port F")
    ]),
    Field("IOPERST", 6, [
        (0, "No effect"),
        (1, "Reset IO port E")
    ]),
    Field("IOPDRST", 5, [
        (0, "No effect"),
        (1, "Reset IO port D")
    ]),
    Field("IOPCRST", 4, [
        (0, "No effect"),
        (1, "Reset IO port C")
    ]),
    Field("IOPBRST", 3, [
        (0, "No effect"),
        (1, "Reset IO port B")
    ]),
    Field("IOPARST", 2, [
        (0, "No effect"),
        (1, "Reset IO port A")
    ]),
    # 1 is reserved
    Field("AFIORST", 21, [
        (0, "No effect"),
        (1, "Reset Alternate Function")
    ])
]

RCC_APB1RSTR = [
    Field("DACRST", 29, [
        (0, "No effect"),
        (1, "Reset DAC interface")
    ]),
    Field("PWRRST", 28, [
        (0, "No effect"),
        (1, "Reset power interface")
    ]),
    Field("BKRST", 27, [
        (0, "No effect"),
        (1, "Reset backup interface")
    ]),
    # 26 reserved
    Field("CANRST", 25, [
        (0, "No effect"),
        (1, "Reset CAN")
    ]),
    # 24 reserved
    Field("USBRST", 23, [
        (0, "No effect"),
        (1, "Reset USB")
    ]),
    Field("I2C2RST", 22, [
        (0, "No effect"),
        (1, "Reset I2C2")
    ]),
    Field("I2C1RST", 21, [
        (0, "No effect"),
        (1, "Reset I2C1")
    ]),
    Field("UART5RST", 20, [
        (0, "No effect"),
        (1, "Reset USART5")
    ]),
    Field("UART4RST", 19, [
        (0, "No effect"),
        (1, "Reset USART5")
    ]),
    Field("UART3RST", 18, [
        (0, "No effect"),
        (1, "Reset USART5")
    ]),
    Field("UART2RST", 17, [
        (0, "No effect"),
        (1, "Reset USART5")
    ]),
    # 16 reserved
    Field("SPI3RST", 15, [
        (0, "No effect"),
        (1, "Reset SPI3")
    ]),
    Field("SPI2RST", 14, [
        (0, "No effect"),
        (1, "Reset SPI2")
    ]),
    # 12-13 reserved
    Field("WWDGRST", 11, [
        (0, "No effect"),
        (1, "Reset window watchdog")
    ]),

    # 9-10 reserved
    Field("TIM14RST", 8, [
        (0, "No effect"),
        (1, "Reset TIM14")
    ]),
    Field("TIM13RST", 7, [
        (0, "No effect"),
        (1, "Reset TIM13")
    ]),
    Field("TIM12RST", 6, [
        (0, "No effect"),
        (1, "Reset TIM12")
    ]),
    Field("TIM7RST", 5, [
        (0, "No effect"),
        (1, "Reset TIM7")
    ]),
    Field("TIM6RST", 4, [
        (0, "No effect"),
        (1, "Reset TIM6")
    ]),
    Field("TIM5RST", 3, [
        (0, "No effect"),
        (1, "Reset TIM5")
    ]),
    Field("TIM4RST", 2, [
        (0, "No effect"),
        (1, "Reset TIM4")
    ]),
    Field("TIM3RST", 1, [
        (0, "No effect"),
        (1, "Reset TIM3")
    ]),
    Field("TIM2RST", 0, [
        (0, "No effect"),
        (1, "Reset TIM2")
    ])    
]

RCC_AHBENR = [
    # 11-31 reserved
    Field("SDIOEN", 10, [
        (0, "SDIO clock disabled"),
        (1, "SDIO clock enabled")
    ]),
    # 9 reserved
    Field("FSMCEN", 8, [
        (0, "FSMC clock disabled"),
        (1, "FSMC clock enabled")
    ]),
    # 7 reserved
    Field("CRCEN", 6, [
        (0, "CRC clock disabled"),
        (1, "CRC clock enabled")
    ]),
    # 5 reserved
    Field("FLITFEN", 4, [
        (0, "FLITF clock disabled during sleep"),
        (1, "FLITF clock enabled during sleep")
    ]),
    # 3 reserved
    Field("SRAMEN", 2, [
        (0, "SRAM clock disabled"),
        (1, "SRAM clock enabled")
    ]),
    Field("DMA2EN", 1, [
        (0, "DMA2 clock disabled"),
        (1, "DMA2 clock enabled")
    ]),
    Field("DMA1EN", 0, [
        (0, "DMA1 clock disabled"),
        (1, "DMA1 clock enabled")
    ])    
]

RCC_APB2ENR = [
    # 22-31 reserved
    Field("TIM11EN", 21, [
        (0, "TIM11 timer clock disabled"),
        (1, "TIM11 timer clock enabled")
    ]),
    Field("TIM10EN", 20, [
        (0, "TIM10 timer clock disabled"),
        (1, "TIM10 timer clock enabled")
    ]),
    Field("TIM9EN", 19, [
        (0, "TIM9 timer clock disabled"),
        (1, "TIM9 timer clock enabled")
    ]),
    # 16-18 reserved
    Field("ADC3EN", 15, [
        (0, "ADC3 interface clock disabled."),
        (1, "ADC3 interface clock enabled.")
    ]),
    Field("USART1EN", 14, [
        (0, "USART1 clock disabled"),
        (1, "USART1 clock enabled")
    ]),
    Field("TIM8EN", 13, [
        (0, "TIM8 timer clock disabled"),
        (1, "TIM8 timer clock enabled")
    ]),
    Field("SPI1EN", 12, [
        (0, "SPI1 clock disabled"),
        (1, "SPI1 clock enabled")
    ]),
    Field("TIM1EN", 11, [
        (0, "TIM1 timer clock disabled"),
        (1, "TIM1 timer clock enabled")
    ]),
    Field("ADC2EN", 10, [
        (0, "ADC2 interface clock disabled"),
        (1, "ADC2 interface clock enabled")
    ]),
    Field("ADC1EN", 9, [
        (0, "ADC1 interface clock disabled"),
        (1, "ADC1 interface clock enabled")
    ]),
    Field("IOPGEN", 8, [
        (0, "IO port G clock disabled"),
        (1, "IO port G clock enabled")
    ]),
    Field("IOPFEN", 7, [
        (0, "IO port F clock disabled"),
        (1, "IO port F clock enabled")
    ]),
    Field("IOPEEN", 6, [
        (0, "IO port E clock disabled"),
        (1, "IO port E clock enabled")
    ]),
    Field("IOPDEN", 5, [
        (0, "IO port D clock disabled"),
        (1, "IO port D clock enabled")
    ]),
    Field("IOPCEN", 4, [
        (0, "IO port C clock disabled"),
        (1, "IO port C clock enabled")
    ]),
    Field("IOPBEN", 3, [
        (0, "IO port B clock disabled"),
        (1, "IO port B clock enabled")
    ]),
    Field("IOPAEN", 2, [
        (0, "IO port A clock disabled"),
        (1, "IO port A clock enabled")
    ]),
    # 1 reserved
    Field("AFIOEN", 0, [
        (0, "Alternate Function IO clock disabled"),
        (1, "Alternate Function IO clock enabled")
    ])
]

RCC_APB1ENR = [
    # 30-31 reserved
    Field("DACEN", 29, [
        (0, "DAC interface clock disabled"),
        (1, "DAC interface clock enabled")
    ]),
    Field("PWREN", 28, [
        (0, "Power interface clock disabled"),
        (1, "Power interface clock enabled")
    ]),
    Field("BKPEN", 27, [
        (0, "Backup interface clock disabled"),
        (1, "Backup interface clock enabled")
    ]),
    # 26 reserved
    Field("CANEN", 25, [
        (0, "CAN clock disabled"),
        (1, "CAN clock enabled")
    ]),
    # 24 reserved
    Field("USBEN", 23, [
        (0, "USB clock disabled"),
        (1, "USB clock enabled")
    ]),
    Field("I2C2EN", 22, [
        (0, "I2C2 clock disabled"),
        (1, "I2C2 clock enabled")
    ]),
    Field("I2C1", 21, [
        (0, "I2C1 clock disabled"),
        (1, "I2C1 clock enabled")
    ]),
    Field("UART5EN", 20, [
        (0, "USART5 clock disabled"),
        (1, "USART5 clock enabled")
    ]),
    Field("UART4EN", 19, [
        (0, "USART4 clock disabled"),
        (1, "USART4 clock enabled")
    ]),
    Field("UART3EN", 18, [
        (0, "USART3 clock disabled"),
        (1, "USART3 clock enabled")
    ]),
    Field("UART2EN", 17, [
        (0, "USART2 clock disabled"),
        (1, "USART2 clock enabled")
    ]),
    # 16 reserved
    Field("SPI3EN", 15, [
        (0, "SPI3 clock disabled"),
        (1, "SPI3 clock enabled")
    ]),
    Field("SPI2", 14, [
        (0, "SPI2 clock disabled"),
        (1, "SPI2 clock enabled")
    ]),
    # 12-13 reserved
    Field("WWDGEN", 11, [
        (0, "Window watchdog clock disabled"),
        (1, "Window watchdog clock enabled")
    ]),
    # 9-10 reserved
    Field("TIM14EN", 8, [
        (0, "TIM14 clock disabled"),
        (1, "TIM14 clock enabled")
    ]),
    Field("TIM13EN", 7, [
        (0, "TIM13 clock disabled"),
        (1, "TIM13 clock enabled")
    ]),
    Field("TIM12EN", 6, [
        (0, "TIM12 clock disabled"),
        (1, "TIM12 clock enabled")
    ]),
    Field("TIM7EN", 5, [
        (0, "TIM7 clock disabled"),
        (1, "TIM7 clock enabled")
    ]),
    Field("TIM6EN", 4, [
        (0, "TIM6 clock disabled"),
        (1, "TIM6 clock enabled")
    ]),
    Field("TIM5EN", 3, [
        (0, "TIM5 clock disabled"),
        (1, "TIM5 clock enabled")
    ]),
    Field("TIM4EN", 2, [
        (0, "TIM4 clock disabled"),
        (1, "TIM4 clock enabled")
    ]),
    Field("TIM3EN", 1, [
        (0, "TIM3 clock disabled"),
        (1, "TIM3 clock enabled")
    ]),
    Field("TIM2EN", 0, [
        (0, "TIM2 clock disabled"),
        (1, "TIM2 clock enabled")
    ])
]

RCC_BDCR = [
    # 17-31 reserved
    Field("BDRST", 16, [
        (0, "Reset not activiated"),
        (1, "Resets the entire Backup domain")
    ]),
    Field("RTCEN", 15, [
        (0, "RTC clock disabled"),
        (1, "RTC clock enabled")
    ]),
    # 10-14 reserved
    Field("RTCSEL", "9:8", [
        (0b00, "No clock"),
        (0b01, "LSE osc clock used at RTC clock"),
        (0b10, "LSI osc clock used at RTC clock"),
        (0b11, "HSE osc clock divided by 128 used as RTC clock")
    ]),
    # 3-7 reserved
    Field("LSEBYP", 2, [
        (0, "LSE osc not bypassed"),
        (1, "LSE osc bypassed")
    ]),
    Field("LSERDY", 1, [
        (0, "External 32kHz osc not ready"),
        (1, "External 32kHz osc ready")
    ]),
    Field("LSEON", 0, [
        (0, "External 32kHz oscillator OFF"),
        (1, "External 32kHz oscillator ON")
    ])
]

RCC_CSR = [
    Field("LPWRRSTF", 31, [
        (0, "No low-power management reset occurred"),
        (1, "Low-power management reset occurred")
    ]),
    Field("WWDGRSTF", 30, [
        (0, "No window watchdog reset occurred"),
        (1, "Window watchdog reset occurred")
    ]),
    Field("IWDGRSTF", 29, [
        (0, "No watchdog reset occurred"),
        (1, "Watchdog reset occurred")
    ]),
    Field("SFTRSTF", 28, [
        (0, "No software reset occurred"),
        (1, "Software reset occurred")
    ]),
    Field("PORRSTF", 27, [
        (0, "No POR/PDR reset occurred"),
        (1, "POR/PDR reset occurred")
    ]),
    Field("PINRSTF", 26, [
        (0, "No reset from NRST pin occurred"),
        (1, "Reset from NRST pin occurred")
    ]),
    # 25 reserved
    Field("RMVF", 24, [
        (0, "No effect"),
        (1, "Clear the reset flags")
    ]),
    # 2-23 reserved
    Field("LSIRDY", 1, [
        (0, "Internal RC 40kHz osc not ready"),
        (1, "Internal RC 40kHz osc ready ")
    ]),
    Field("LSION", 0, [
        (0, "Internal RC 40kHz osc OFF"),
        (1, "Internal RC 40kHz osc ON")
    ])
]



class RegStructPrinter(object):
    def __init__(self, name, val, regs):
        self.name = name
        self.val = val
        self.regs = regs

    def to_string(self):
        return self.name + ":"

    #def display_hint(self):
    #    return "map"
    
    def children(self):
        for reg, fieldlist in self.regs:
            yield reg, invokePrintersOnFields(self.val[reg], fieldlist)


outputspeeds = {
    0b01: "10Mhz",
    0b10: "2Mhz",
    0b11: "50Mhz"
    }

outputcnf = {
    0b00: "Push/Pull",
    0b01: "Open-drain",
    0b10: "Alternate push/pull",
    0b11: "Alternate open-drain"
    }

inputcnf = {
    0b00: "Analog",
    0b01: "Floating",
    0b10: "Input with pull up/down",
    0b11: "Reserved"
    }

# r = re.compile("^TYPE_CODE")
# types = [key for key in dir(gdb) if (r.match(key))]
# for type in types:
#     print("type {} {}".format(type, getattr(gdb, type)))
    

class GPIOPrinter(object):
    "Print a GPIO_Typedef"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        "GPIO:"

    def children(self):

        idr = self.val['IDR']
        odr = self.val['ODR']

        retval = []
        for cr, offset in (self.val['CRL'],0), (self.val['CRH'],8):
            for i in range(8):
                mode = (cr >> (i*4)) & 0b11
                cnf  = (cr >> (i*4+2)) & 0b11
                
                if (mode == 0):
                    # input mode
                    inputval = (idr >> (i+offset)) & 0b1
                    yield str(i+offset), "Input {} {}".format(inputcnf[int(cnf)], inputval)

                else:
                    outputval = (odr >> (i+offset)) & 0b1
                    yield str(i+offset), "Output {} {}:{}".format(outputspeeds[int(mode)],
                                                                  outputcnf[int(cnf)],
                                                                  outputval)

    def display_hint(self):
        return 'string'


def stm32_lookup_function(val):
    "Look-up and return a pretty-printer that can print val."
    # Get the type.
    type = val.type

    # If it points to a reference, get the reference.
    if type.code == gdb.TYPE_CODE_REF:
        type = type.target ()

    if type.code == gdb.TYPE_CODE_PTR:
        val = val.dereference()
        type = type.target ()
        
    # Get the unqualified type, stripped of typedefs.
    # don't want to do this. we'd lose the name of the struct
    # type = type.unqualified ().strip_typedefs ()

    # Get the type name. Other samples use .tag, but that yields
    # None for the ones I've tried.
    typename = type.name
    if typename == None:
        return None
 
    regex = re.compile("^GPIO_TypeDef$")
    if regex.match(typename):
        return GPIOPrinter(val)

    regex = re.compile("^RCC_TypeDef$")
    if regex.match(typename):
        return RegStructPrinter("RCC", val, [
            ('CR', RCC_CR),
            ('CFGR', RCC_CFGR),
            ('CIR', RCC_CIR),
            ('APB2RSTR', RCC_APB2RSTR),
            ('APB1RSTR', RCC_APB1RSTR),
            ('AHBENR', RCC_AHBENR),
            ('APB2ENR', RCC_APB2ENR),
            ('APB1ENR', RCC_APB1ENR),
            ('BDCR', RCC_BDCR),
            ('CSR', RCC_CSR)
            ])
    return None

def register_printers(obj):
    print("registering printers")
    gdb.pretty_printers.append(stm32_lookup_function)
    

print("loaded")
