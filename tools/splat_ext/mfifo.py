#!/usr/bin/env python3

from splat.segtypes.common.data import CommonSegData

class PS2SegMfifo(CommonSegData):
    def get_linker_section(self) -> str:
        return ".mfifo"

    def get_section_flags(self):
        return "wa"
