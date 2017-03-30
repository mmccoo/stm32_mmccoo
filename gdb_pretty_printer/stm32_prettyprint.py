
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

class RC_CRPrinter(object):
    def __init__(self, val):
        self.val = val

    def to_string(self):
        "CR:"

    def children(self):
        for field in RCC_CR:
            k = field.name
            v = field.to_string(self.val)
            yield field.name, v

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
            ('CFGR', RCC_CFGR)
            ])
    return None

def register_printers(obj):
    print("registering printers")
    gdb.pretty_printers.append(stm32_lookup_function)
    

print("loaded")
