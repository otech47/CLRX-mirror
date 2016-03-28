/*
 *  CLRadeonExtender - Unofficial OpenCL Radeon Extensions Library
 *  Copyright (C) 2014-2016 Mateusz Szpakowski
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <CLRX/Config.h>
#include <iostream>
#include <sstream>
#include <map>
#include <memory>
#include <CLRX/utils/Containers.h>
#include <CLRX/amdbin/AmdCL2Binaries.h>
#include <CLRX/amdbin/AmdCL2BinGen.h>

using namespace CLRX;

static const char* origBinaryFiles[] =
{
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/alltypes-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/alltypes.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/atomics.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/atomics2.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/BinarySearchDeviceSideEnqueue_Kernels.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/enqueue-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/enqueue.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/ExtractPrimes_Kernels.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/ksetup1-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/ksetup1.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/ksetup2-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/ksetup2.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/locals-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/locals.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/nokernels-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/nokernels.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/piper-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/piper.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/samplers2.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/samplers4.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/scratch-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/scratch.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/test2-15_7.clo",
    CLRX_SOURCE_DIR "/tests/amdbin/amdcl2bins/test2.clo"
};

struct IntAmdCL2SetupData
{
    uint32_t pgmRSRC1;
    uint32_t pgmRSRC2;
    uint16_t setup1;
    uint16_t archInd;
    uint32_t scratchBufferSize;
    uint32_t localSize; // in bytes
    uint32_t zero1;
    uint32_t kernelArgsSize;
    uint32_t zeroes[2];
    uint16_t sgprsNumAll;
    uint16_t vgprsNum16;
    uint32_t vgprsNum;
    uint32_t sgprsNum;
    uint32_t zero3;
    uint32_t version; // ??
};

typedef std::pair<const char*, KernelArgType> ArgTypeNameEntry;

static const ArgTypeNameEntry cl20ArgNameTypeTable[] =
{
    { "char", KernelArgType::CHAR },
    { "char16", KernelArgType::CHAR16 },
    { "char2", KernelArgType::CHAR2 },
    { "char3", KernelArgType::CHAR3 },
    { "char4", KernelArgType::CHAR4 },
    { "char8", KernelArgType::CHAR8 },
    { "clk_event_t", KernelArgType::CLKEVENT },
    { "double", KernelArgType::DOUBLE },
    { "double16", KernelArgType::DOUBLE16 },
    { "double2", KernelArgType::DOUBLE2 },
    { "double3", KernelArgType::DOUBLE3 },
    { "double4", KernelArgType::DOUBLE4 },
    { "double8", KernelArgType::DOUBLE8 },
    { "float", KernelArgType::FLOAT },
    { "float16", KernelArgType::FLOAT16 },
    { "float2", KernelArgType::FLOAT2 },
    { "float3", KernelArgType::FLOAT3 },
    { "float4", KernelArgType::FLOAT4 },
    { "float8", KernelArgType::FLOAT8 },
    { "image1d_array_t", KernelArgType::IMAGE1D_ARRAY },
    { "image1d_buffer_t", KernelArgType::IMAGE1D_BUFFER },
    { "image1d_t", KernelArgType::IMAGE1D },
    { "image2d_array_t", KernelArgType::IMAGE2D_ARRAY },
    { "image2d_t", KernelArgType::IMAGE2D },
    { "image3d_t", KernelArgType::IMAGE2D },
    { "int", KernelArgType::INT },
    { "int16", KernelArgType::INT16 },
    { "int2", KernelArgType::INT2 },
    { "int3", KernelArgType::INT3 },
    { "int4", KernelArgType::INT4 },
    { "int8", KernelArgType::INT8 },
    { "long", KernelArgType::LONG },
    { "long16", KernelArgType::LONG16 },
    { "long2", KernelArgType::LONG2 },
    { "long3", KernelArgType::LONG3 },
    { "long4", KernelArgType::LONG4 },
    { "long8", KernelArgType::LONG8 },
    { "pipe", KernelArgType::PIPE },
    { "queue_t", KernelArgType::CMDQUEUE },
    { "sampler_t", KernelArgType::SAMPLER },
    { "short", KernelArgType::SHORT },
    { "short16", KernelArgType::SHORT16 },
    { "short2", KernelArgType::SHORT2 },
    { "short3", KernelArgType::SHORT3 },
    { "short4", KernelArgType::SHORT4 },
    { "short8", KernelArgType::SHORT8 },
    { "size_t", KernelArgType::ULONG },
    { "uchar", KernelArgType::UCHAR },
    { "uchar16", KernelArgType::UCHAR16 },
    { "uchar2", KernelArgType::UCHAR2 },
    { "uchar3", KernelArgType::UCHAR3 },
    { "uchar4", KernelArgType::UCHAR4 },
    { "uchar8", KernelArgType::UCHAR8 },
    { "uint", KernelArgType::UINT },
    { "uint16", KernelArgType::UINT16 },
    { "uint2", KernelArgType::UINT2 },
    { "uint3", KernelArgType::UINT3 },
    { "uint4", KernelArgType::UINT4 },
    { "uint8", KernelArgType::UINT8 },
    { "ulong", KernelArgType::ULONG },
    { "ulong16", KernelArgType::ULONG16 },
    { "ulong2", KernelArgType::ULONG2 },
    { "ulong3", KernelArgType::ULONG3 },
    { "ulong4", KernelArgType::ULONG4 },
    { "ulong8", KernelArgType::ULONG8 },
    { "ushort", KernelArgType::USHORT },
    { "ushort16", KernelArgType::USHORT16 },
    { "ushort2", KernelArgType::USHORT2 },
    { "ushort3", KernelArgType::USHORT3 },
    { "ushort4", KernelArgType::USHORT4 },
    { "ushort8", KernelArgType::USHORT8 },
    { "void", KernelArgType::VOID }
};

static const size_t cl20ArgNameTypeTableSize = sizeof(cl20ArgNameTypeTable)/
            sizeof(ArgTypeNameEntry);

static const KernelArgType cl20ArgTypeVectorTable[] =
{
    KernelArgType::CHAR,
    KernelArgType::CHAR2,
    KernelArgType::CHAR3,
    KernelArgType::CHAR4,
    KernelArgType::CHAR8,
    KernelArgType::CHAR16,
    KernelArgType::SHORT,
    KernelArgType::SHORT2,
    KernelArgType::SHORT3,
    KernelArgType::SHORT4,
    KernelArgType::SHORT8,
    KernelArgType::SHORT16,
    KernelArgType::INT,
    KernelArgType::INT2,
    KernelArgType::INT3,
    KernelArgType::INT4,
    KernelArgType::INT8,
    KernelArgType::INT16,
    KernelArgType::LONG,
    KernelArgType::LONG2,
    KernelArgType::LONG3,
    KernelArgType::LONG4,
    KernelArgType::LONG8,
    KernelArgType::LONG16,
    KernelArgType::VOID,
    KernelArgType::VOID,
    KernelArgType::VOID,
    KernelArgType::VOID,
    KernelArgType::VOID,
    KernelArgType::VOID,
    KernelArgType::FLOAT,
    KernelArgType::FLOAT2,
    KernelArgType::FLOAT3,
    KernelArgType::FLOAT4,
    KernelArgType::FLOAT8,
    KernelArgType::FLOAT16,
    KernelArgType::DOUBLE,
    KernelArgType::DOUBLE2,
    KernelArgType::DOUBLE3,
    KernelArgType::DOUBLE4,
    KernelArgType::DOUBLE8,
    KernelArgType::DOUBLE16
};

static const cxuint vectorIdTable[17] =
{ UINT_MAX, 0, 1, 2, 3, UINT_MAX, UINT_MAX, UINT_MAX, 4,
  UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 5 };

static AmdCL2KernelConfig genKernelConfig(size_t metadataSize, const cxbyte* metadata,
        size_t setupSize, const cxbyte* setup, const std::vector<size_t> samplerOffsets,
        const std::vector<AmdCL2RelInput>& textRelocs)
{
    AmdCL2KernelConfig config;
    const AmdCL2GPUMetadataHeader* mdHdr =
            reinterpret_cast<const AmdCL2GPUMetadataHeader*>(metadata);
    size_t headerSize = ULEV(mdHdr->size);
    for (size_t i = 0; i < 3; i++)
        config.reqdWorkGroupSize[i] = ULEV(mdHdr->reqdWorkGroupSize[i]);
    const IntAmdCL2SetupData* setupData =
            reinterpret_cast<const IntAmdCL2SetupData*>(setup + 48);
    uint32_t pgmRSRC1 = ULEV(setupData->pgmRSRC1);
    uint32_t pgmRSRC2 = ULEV(setupData->pgmRSRC2);
    config.dimMask = (pgmRSRC2>>7)&7;
    config.ieeeMode = (pgmRSRC1>>23)&1;
    config.exceptions = (pgmRSRC2>>24)&0xff;
    config.floatMode = (pgmRSRC1>>12)&0xff;
    config.priority = (pgmRSRC1>>10)&3;
    config.tgSize = (pgmRSRC2>>10)&1;
    config.privilegedMode = (pgmRSRC1>>20)&1;
    config.dx10Clamp = (pgmRSRC1>>21)&1;
    config.debugMode = (pgmRSRC1>>22)&1;
    config.pgmRSRC2 = pgmRSRC2;
    config.pgmRSRC1 = pgmRSRC1;
    config.usedVGPRsNum = ULEV(setupData->vgprsNum);
    config.usedSGPRsNum = ULEV(setupData->sgprsNum);
    config.scratchBufferSize = ULEV(setupData->scratchBufferSize);
    config.localSize = ULEV(setupData->localSize);
    uint16_t ksetup1 = ULEV(setupData->setup1);
    config.useSizes = (ksetup1&2)!=0;
    config.useSetup = (ksetup1&8)!=0;
    config.useEnqueue = (ksetup1&0x20)!=0;
    // get samplers
    for (const AmdCL2RelInput& reloc: textRelocs)
    {   // check if sampler
        auto it = std::find(samplerOffsets.begin(), samplerOffsets.end(), reloc.addend);
        if (it!=samplerOffsets.end())
            config.samplers.push_back(it-samplerOffsets.begin());
    }
    std::sort(config.samplers.begin(), config.samplers.end());
    config.samplers.resize(std::unique(config.samplers.begin(), config.samplers.end()) -
                config.samplers.begin());
    // get kernel args
    size_t argOffset = headerSize + ULEV(mdHdr->firstNameLength) + 
            ULEV(mdHdr->secondNameLength)+2;
    const AmdCL2GPUKernelArgEntry* argPtr = reinterpret_cast<
            const AmdCL2GPUKernelArgEntry*>(metadata + argOffset);
    const uint32_t argsNum = ULEV(mdHdr->argsNum);
    const char* strBase = (const char*)metadata;
    size_t strOffset = argOffset + sizeof(AmdCL2GPUKernelArgEntry)*(argsNum+1);
    
    for (uint32_t i = 0; i < argsNum; i++, argPtr++)
    {
        AmdKernelArgInput arg{};
        size_t nameSize = ULEV(argPtr->argNameSize);
        arg.argName.assign(strBase+strOffset, nameSize);
        strOffset += nameSize+1;
        nameSize = ULEV(argPtr->typeNameSize);
        arg.typeName.assign(strBase+strOffset, nameSize);
        strOffset += nameSize+1;
        
        uint32_t vectorSize = ULEV(argPtr->vectorLength);
        uint32_t argType = ULEV(argPtr->argType);
        uint32_t kindOfType = ULEV(argPtr->kindOfType);
        
        arg.ptrSpace = KernelPtrSpace::NONE;
        arg.ptrAccess = KARG_PTR_NORMAL;
        arg.argType = KernelArgType::VOID;
        
        if (ULEV(argPtr->isConst))
            arg.ptrAccess |= KARG_PTR_CONST;
        
        if (!ULEV(argPtr->isPointerOrPipe))
        { // if not point or pipe (get regular type: scalar, image, sampler,...)
            switch(argType)
            {
                case 0:
                    if (kindOfType!=1) // not sampler
                        throw Exception("Wrong kernel argument type");
                    arg.argType = KernelArgType::SAMPLER;
                    break;
                case 1:  // read_only image
                case 2:  // write_only image
                case 3:  // read_write image
                    if (kindOfType!=2) // not image
                        throw Exception("Wrong kernel argument type");
                    arg.argType = KernelArgType::IMAGE;
                    arg.ptrAccess |= (argType==1) ? KARG_PTR_READ_ONLY : (argType==2) ?
                             KARG_PTR_WRITE_ONLY : KARG_PTR_READ_WRITE;
                    arg.ptrSpace = KernelPtrSpace::GLOBAL;
                    break;
                case 6: // char
                case 7: // short
                case 8: // int
                case 9: // long
                case 11: // float
                case 12: // double
                {
                    if (kindOfType!=4) // not scalar
                        throw Exception("Wrong kernel argument type");
                    const cxuint vectorId = vectorIdTable[vectorSize];
                    if (vectorId == UINT_MAX)
                        throw Exception("Wrong vector size");
                    arg.argType = cl20ArgTypeVectorTable[(argType-6)*6 + vectorId];
                    break;
                }
                case 15:
                    if (kindOfType!=4) // not scalar
                        throw Exception("Wrong kernel argument type");
                    arg.argType = KernelArgType::STRUCTURE;
                    break;
                case 18:
                    if (kindOfType!=7) // not scalar
                        throw Exception("Wrong kernel argument type");
                    arg.argType = KernelArgType::CMDQUEUE;
                    break;
                default:
                    throw Exception("Wrong kernel argument type");
            }
            
            auto it = binaryMapFind(cl20ArgNameTypeTable,
                        cl20ArgNameTypeTable + cl20ArgNameTypeTableSize,
                        arg.typeName.c_str(), CStringLess());
            if (it != cl20ArgNameTypeTable + cl20ArgNameTypeTableSize) // if found
                arg.argType = it->second;
            
            if (arg.argType == KernelArgType::STRUCTURE)
                arg.structSize = ULEV(argPtr->structSize);
            else if ((arg.argType >= KernelArgType::MIN_IMAGE &&
                      arg.argType <= KernelArgType::MAX_IMAGE) ||
                     arg.argType == KernelArgType::SAMPLER)
                arg.resId = LEV(argPtr->resId);
        }
        else
        {   // pointer or pipe
            if (argPtr->isPipe)
                arg.used = (ULEV(argPtr->isPointerOrPipe))==3;
            else
                arg.used = (ULEV(argPtr->isPointerOrPipe));
            uint32_t ptrType = ULEV(argPtr->ptrType);
            uint32_t ptrSpace = ULEV(argPtr->ptrSpace);
            if (argType == 7) // pointer
                arg.argType = (argPtr->isPipe==0) ? KernelArgType::POINTER :
                    KernelArgType::PIPE;
            else if (argType == 15)
                arg.argType = KernelArgType::POINTER;
            if (arg.argType == KernelArgType::POINTER)
            {   // if pointer
                if (ptrSpace==3)
                    arg.ptrSpace = KernelPtrSpace::LOCAL;
                else if (ptrSpace==4)
                    arg.ptrSpace = KernelPtrSpace::GLOBAL;
                else if (ptrSpace==5)
                    arg.ptrSpace = KernelPtrSpace::CONSTANT;
                else
                    throw Exception("Illegal pointer space");
                // set access qualifiers (volatile, restrict, const)
                arg.ptrAccess |= KARG_PTR_NORMAL;
                if (argPtr->isRestrict)
                    arg.ptrAccess |= KARG_PTR_RESTRICT;
                if (argPtr->isVolatile)
                    arg.ptrAccess |= KARG_PTR_VOLATILE;
            }
            else
            {
                if (ptrSpace!=4)
                    throw Exception("Illegal pipe space");
                arg.ptrSpace = KernelPtrSpace::GLOBAL;
            }
            
            if (arg.argType == KernelArgType::POINTER)
            {
                size_t ptrTypeNameSize=0;
                if (arg.typeName.empty()) // ctx_struct_fld1
                {
                    if (ptrType >= 6 && ptrType <= 12)
                        arg.pointerType = cl20ArgTypeVectorTable[(ptrType-6)*6];
                    else
                        arg.pointerType = KernelArgType::FLOAT;
                }
                else
                {
                    while (isAlnum(arg.typeName[ptrTypeNameSize]) ||
                        arg.typeName[ptrTypeNameSize]=='_') ptrTypeNameSize++;
                    CString ptrTypeName(arg.typeName.c_str(), ptrTypeNameSize);
                    if (arg.typeName.find('*')!=CString::npos) // assume 'void*'
                    {
                        auto it = binaryMapFind(cl20ArgNameTypeTable,
                                    cl20ArgNameTypeTable + cl20ArgNameTypeTableSize,
                                    ptrTypeName.c_str(), CStringLess());
                        if (it != cl20ArgNameTypeTable + cl20ArgNameTypeTableSize)
                            // if found
                            arg.pointerType = it->second;
                        else // otherwise structure
                            arg.pointerType = KernelArgType::STRUCTURE;
                    }
                    else if (ptrType==18)
                    {
                        arg.argType = KernelArgType::CLKEVENT;
                        arg.pointerType = KernelArgType::VOID;
                    }
                    else
                        arg.pointerType = KernelArgType::VOID;
                        
                    if (arg.pointerType == KernelArgType::STRUCTURE)
                        arg.structSize = ULEV(argPtr->ptrAlignment);
                }
            }
        }
        config.args.push_back(arg);
    }
    return config;
}

struct CLRX_INTERNAL CL2GPUDeviceCodeEntry
{
    uint32_t elfFlags;
    GPUDeviceType deviceType;
};

static const CL2GPUDeviceCodeEntry cl2GpuDeviceCodeTable[11] =
{
    { 6, GPUDeviceType::BONAIRE },
    { 1, GPUDeviceType::SPECTRE },
    { 2, GPUDeviceType::SPOOKY },
    { 3, GPUDeviceType::KALINDI },
    { 7, GPUDeviceType::HAWAII },
    { 8, GPUDeviceType::ICELAND },
    { 9, GPUDeviceType::TONGA },
    { 4, GPUDeviceType::MULLINS },
    { 17, GPUDeviceType::FIJI },
    { 16, GPUDeviceType::CARRIZO },
    { 15, GPUDeviceType::DUMMY }
};

static AmdCL2Input genAmdCL2Input(bool useConfig, const AmdCL2MainGPUBinary& binary,
            bool addBrig, bool samplerConfig)
{
    AmdCL2Input amdCL2Input{ };
    cxuint index;
    const uint32_t elfFlags = ULEV(binary.getHeader().e_flags);
    for (index = 0; index < 11; index++)
        if (cl2GpuDeviceCodeTable[index].elfFlags == elfFlags)
            break;
    
    amdCL2Input.deviceType = cl2GpuDeviceCodeTable[index].deviceType;
    amdCL2Input.aclVersion = binary.getAclVersionString();
    //std::cout << "BinCompOptions: " << binary.getCompileOptions() << std::endl;
    amdCL2Input.compileOptions = binary.getCompileOptions();
    bool isNewBinary = (binary.hasInnerBinary() &&
            binary.getInnerBinaryType()==AmdCL2InnerBinaryType::NEW);
    amdCL2Input.driverVersion = isNewBinary ? 191205 : 180005;
    
    amdCL2Input.samplerInitSize = 0;
    amdCL2Input.samplerInit = nullptr;
    if (isNewBinary)
    {
        const AmdCL2InnerGPUBinary& innerBin = binary.getInnerBinary();
        amdCL2Input.globalDataSize = innerBin.getGlobalDataSize();
        amdCL2Input.globalData = innerBin.getGlobalData();
        amdCL2Input.rwDataSize = innerBin.getRwDataSize();
        amdCL2Input.rwData = innerBin.getRwData();
        amdCL2Input.bssAlignment = innerBin.getBssAlignment();
        amdCL2Input.bssSize = innerBin.getBssSize();
        amdCL2Input.samplerConfig = samplerConfig;
        if (samplerConfig)
        {
            const uint32_t* sampEntry = (const uint32_t*)innerBin.getSamplerInit();
            for (size_t i = 0; i < innerBin.getSamplerInitSize()/8; i++)
                amdCL2Input.samplers.push_back(ULEV(sampEntry[i*2+1]));
        }
        else
        {
            amdCL2Input.samplerInitSize = innerBin.getSamplerInitSize();
            amdCL2Input.samplerInit = innerBin.getSamplerInit();
        }
        const size_t sampRelsNum = innerBin.getGlobalDataRelaEntriesNum();
        for (size_t k = 0; k < sampRelsNum; k++)
        {
            const Elf64_Rela& rela = innerBin.getGlobalDataRelaEntry(k);
            amdCL2Input.samplerOffsets.push_back(ULEV(rela.r_offset));
        }
    }
    
    size_t kernelsNum = binary.getKernelInfosNum();
    const AmdCL2InnerGPUBinaryBase& innerBinBase = binary.getInnerBinaryBase();
    for (size_t k = 0; k < kernelsNum; k++)
    {
        const KernelInfo& kinfo = binary.getKernelInfo(k);
        AmdCL2KernelInput kernel;
        kernel.kernelName = kinfo.kernelName;
        kernel.metadataSize = binary.getMetadataSize(k);
        kernel.metadata = binary.getMetadata(k);
        kernel.isaMetadataSize = 0;
        kernel.isaMetadata = nullptr;
        kernel.useConfig = useConfig;
        if (!isNewBinary)
        {
            kernel.isaMetadataSize = binary.getISAMetadataSize(k);
            kernel.isaMetadata = binary.getISAMetadata(k);
        }
        const AmdCL2GPUKernel& kdata = innerBinBase.getKernelData(k);
        kernel.stubSize = 0;
        kernel.stub = nullptr;
        kernel.setupSize = kdata.setupSize;
        kernel.setup = kdata.setup;
        kernel.codeSize = kdata.codeSize;
        kernel.code = kdata.code;
        amdCL2Input.kernels.push_back(kernel);
    }
    
    if (!isNewBinary)
    {   // old binary
        const AmdCL2OldInnerGPUBinary& innerBin = binary.getOldInnerBinary();
        for (size_t k = 0; k < kernelsNum; k++)
        {
            AmdCL2KernelInput& kernel = amdCL2Input.kernels[k];
            const AmdCL2GPUKernelStub& stub = innerBin.getKernelStub(k);
            kernel.stubSize = stub.size;
            kernel.stub = stub.data;
        }
    }
    else
    {   // new binary
        const AmdCL2InnerGPUBinary& innerBin = binary.getInnerBinary();
        const size_t textRelsNum = innerBin.getTextRelaEntriesNum();
        size_t relaId = 0;
        size_t offset = 0;
        
        size_t gDataSymIndex = 0;
        size_t aDataSymIndex = 0;
        size_t bssSymIndex = 0;
        const size_t symbolsNum = innerBin.getSymbolsNum();
        for (gDataSymIndex = 0; gDataSymIndex < symbolsNum; gDataSymIndex++)
        {   // find global data symbol (getSymbolIndex doesn't work always
            const char* name = innerBin.getSymbolName(gDataSymIndex);
            if (::strcmp(name, "__hsa_section.hsadata_readonly_agent")==0)
                break;
        }
        for (aDataSymIndex = 0; aDataSymIndex < symbolsNum; aDataSymIndex++)
        {   // find global data symbol (getSymbolIndex doesn't work always
            const char* name = innerBin.getSymbolName(aDataSymIndex);
            if (::strcmp(name, "__hsa_section.hsadata_global_agent")==0)
                break;
        }
        for (bssSymIndex = 0; bssSymIndex < symbolsNum; bssSymIndex++)
        {   // find global data symbol (getSymbolIndex doesn't work always
            const char* name = innerBin.getSymbolName(bssSymIndex);
            if (::strcmp(name, "__hsa_section.hsabss_global_agent")==0)
                break;
        }
        
        for (size_t k = 0; k < kernelsNum; k++)
        {
            AmdCL2KernelInput& kernel = amdCL2Input.kernels[k];
            offset += kernel.setupSize;
            for (; relaId < textRelsNum; relaId++)
            {
                const Elf64_Rela& rela = innerBin.getTextRelaEntry(relaId);
                if (ULEV(rela.r_offset) < offset)
                    continue;
                if (ULEV(rela.r_offset) >= offset + kernel.codeSize)
                    break;
                cxuint typev = ELF64_R_TYPE(ULEV(rela.r_info));
                uint32_t symIndex = ELF64_R_SYM(ULEV(rela.r_info));
                RelocType rtype;
                if (typev == 1)
                    rtype = RelocType::LOW_32BIT;
                else if (typev == 2)
                    rtype = RelocType::HIGH_32BIT;
                else
                    throw Exception("Wrong reltype");
                cxuint rsym = symIndex==aDataSymIndex?1U:(symIndex==bssSymIndex?2U:0U);
                kernel.relocations.push_back({size_t(ULEV(rela.r_offset))-offset,
                        rtype, rsym, size_t(ULEV(rela.r_addend))});
            }
            offset += kernel.codeSize;
            if ((offset & 255) != 0)
                offset += 256 - (offset & 255);
        }
    }
    
    if (useConfig)
        for (AmdCL2KernelInput& kernel: amdCL2Input.kernels)
        {
            kernel.config = genKernelConfig(kernel.metadataSize, kernel.metadata,
                        kernel.setupSize, kernel.setup, amdCL2Input.samplerOffsets,
                        kernel.relocations);
        }
    
    // add brig
    uint16_t brigIndex = binary.getSectionIndex(".brig");
    const Elf64_Shdr& brigShdr = binary.getSectionHeader(brigIndex);
    const cxbyte* brigContent = binary.getSectionContent(brigIndex);
    amdCL2Input.extraSections.push_back(BinSection{".brig", size_t(ULEV(brigShdr.sh_size)),
            brigContent, size_t(ULEV(brigShdr.sh_addralign)), ULEV(brigShdr.sh_type),
            ULEV(brigShdr.sh_flags), ELFSECTID_NULL, ULEV(brigShdr.sh_info),
            size_t(ULEV(brigShdr.sh_entsize))});
    return amdCL2Input;
}

static void testOrigBinary(cxuint testCase, const char* origBinaryFilename, bool reconf)
{
    Array<cxbyte> inputData;
    Array<cxbyte> output;
    AmdCL2Input amdCL2Input;
    
    inputData = loadDataFromFile(origBinaryFilename);
    if (!isAmdCL2Binary(inputData.size(), inputData.data()))
        throw Exception("This is not AMD OpenCL2.0 binary");
    AmdCL2MainGPUBinary amdCL2GpuBin(inputData.size(), inputData.data(), 
                AMDBIN_CREATE_KERNELINFO | AMDBIN_CREATE_KERNELINFOMAP |
                AMDBIN_CREATE_INNERBINMAP | AMDBIN_CREATE_KERNELHEADERS |
                AMDBIN_CREATE_KERNELHEADERMAP | AMDBIN_CREATE_INFOSTRINGS |
                AMDBIN_INNER_CREATE_KERNELDATA | AMDBIN_INNER_CREATE_KERNELDATAMAP |
                AMDBIN_INNER_CREATE_KERNELSTUBS);
    
    amdCL2Input = genAmdCL2Input(reconf, amdCL2GpuBin, false, false);
    
    AmdCL2GPUBinGenerator binGen(&amdCL2Input);
    binGen.generate(output);
    
    if (output.size() != inputData.size())
    {
        std::ostringstream oss;
        oss << "Failed for #" << testCase << " file=" << origBinaryFilename <<
                ": expectedSize=" << inputData.size() <<
                ", resultSize=" << output.size();
        throw Exception(oss.str());
    }
    for (size_t i = 0; i < inputData.size(); i++)
        if (output[i] != inputData[i])
        {
            std::ostringstream oss;
            oss << "Failed for #" << testCase << " file=" << origBinaryFilename <<
                    ": byte=" << i;
            throw Exception(oss.str());
        }
}

int main(int argc, const char** argv)
{
    int retVal = 0;
    for (cxuint i = 0; i < sizeof(origBinaryFiles)/sizeof(const char*); i++)
    {
        std::string regenName = origBinaryFiles[i];
        filesystemPath(regenName); // convert to system path (native separators)
        regenName += ".regen";
        std::string reconfName = origBinaryFiles[i];
        filesystemPath(reconfName); // convert to system path (native separators)
        reconfName += ".reconf";
        try
        { testOrigBinary(i, regenName.c_str(), false); }
        catch(const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            retVal = 1;
        }
        try
        { testOrigBinary(i, reconfName.c_str(), true); }
        catch(const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            retVal = 1;
        }
    }
    return retVal;
}