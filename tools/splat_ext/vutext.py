#!/usr/bin/env python3

from splat.segtypes.common.databin import CommonSegDatabin

class PS2SegVutext(CommonSegDatabin):
    def get_linker_section(self) -> str:
        return ".vutext"

    def get_section_flags(self):
        return "ax"
