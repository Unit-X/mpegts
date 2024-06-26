#pragma once

// Prefixes used
// m class member
// p pointer (*)
// r reference (&)
// l local scope

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "common.h"

namespace mpegts {

static constexpr size_t kTsPacketSize_188 = 188;
static constexpr uint8_t kTsPacketSyncByte = 0x47;

class EsFrame final {
public:
    EsFrame() = default;

    EsFrame(uint8_t streamType, uint16_t pid);

    ~EsFrame() = default;

    [[nodiscard]] bool empty() const;

    void reset();

    std::shared_ptr<SimpleBuffer> mData = std::make_shared<SimpleBuffer>();
    uint64_t mPts = 0;
    uint64_t mDts = 0;
    uint64_t mPcr = 0;
    uint8_t mRandomAccess = 0;
    uint8_t mStreamType = 0;
    uint8_t mStreamId = 0;
    uint16_t mPid = 0;
    uint16_t mExpectedPesPacketLength = 0;
    uint16_t mExpectedPayloadLength = 0;
    bool mCompleted = false;
    bool mBroken = false;
};

class TsHeader final {
public:
    TsHeader() = default;

    ~TsHeader() = default;

    void encode(SimpleBuffer& rSb) const;

    void decode(SimpleBuffer& rSb);

    [[nodiscard]] bool hasPayload() const;
    [[nodiscard]] bool hasAdaptationField() const;

    uint8_t mSyncByte = kTsPacketSyncByte;   // 8 bits
    uint8_t mTransportErrorIndicator = 0;    // 1 bit
    uint8_t mPayloadUnitStartIndicator = 0;  // 1 bit
    uint8_t mTransportPriority = 0;          // 1 bit
    uint16_t mPid = 0;                       // 13 bits
    uint8_t mTransportScramblingControl = 0; // 2 bits
    uint8_t mAdaptationFieldControl = 0;     // 2 bits
    uint8_t mContinuityCounter = 0;          // 4 bits
};

class PATHeader final {
public:
    PATHeader() = default;

    ~PATHeader() = default;

    void encode(SimpleBuffer& rSb) const;

    void decode(SimpleBuffer& rSb);

    void print(LogLevel logLevel, const std::function<void(LogLevel level, const std::string&)>& streamInfoCallback) const;

    uint8_t mTableId = 0;                // 8 bits
    uint8_t mSectionSyntaxIndicator = 0; // 1 bit
    uint8_t mB0 = 0;                     // 1 bit
    uint8_t mReserved0 = 0;              // 2 bits
    uint16_t mSectionLength = 0;         // 12 bits
    uint16_t mTransportStreamId = 0;     // 16 bits
    uint8_t mReserved1 = 0;              // 2 bits
    uint8_t mVersionNumber = 0;          // 5 bits
    uint8_t mCurrentNextIndicator = 0;   // 1 bit
    uint8_t mSectionNumber = 0;          // 8 bits
    uint8_t mLastSectionNumber = 0;      // 8 bits
};

class PMTElementInfo final {
public:
    PMTElementInfo() = default;

    PMTElementInfo(uint8_t lSt, uint16_t lPid);

    ~PMTElementInfo() = default;

    void encode(SimpleBuffer& rSb) const;

    void decode(SimpleBuffer& rSb);

    [[nodiscard]] uint16_t size() const;

    void print(LogLevel logLevel, const std::function<void(LogLevel level, const std::string&)>& streamInfoCallback) const;

    uint8_t mStreamType = 0;     // 8 bits
    uint8_t mReserved0 = 0x7;    // 3 bits
    uint16_t mElementaryPid = 0; // 13 bits
    uint8_t mReserved1 = 0xf;    // 4 bits
    uint16_t mEsInfoLength = 0;  // 12 bits
    std::string mEsInfo;
};

class PMTHeader final {
public:
    PMTHeader() = default;

    ~PMTHeader() = default;

    void encode(SimpleBuffer& rSb);

    void decode(SimpleBuffer& rSb);

    [[nodiscard]] uint16_t size() const;

    void print(LogLevel logLevel, const std::function<void(LogLevel level, const std::string&)>& streamInfoCallback);

    uint8_t mTableId = 0x02;             // 8 bits
    uint8_t mSectionSyntaxIndicator = 0; // 1 bit
    uint8_t mB0 = 0;                     // 1 bit
    uint8_t mReserved0 = 0;              // 2 bits
    uint16_t mSectionLength = 0;         // 12 bits
    uint16_t mProgramNumber = 0;         // 16 bits
    uint8_t mReserved1 = 0;              // 2 bits
    uint8_t mVersionNumber = 0;          // 5 bits
    uint8_t mCurrentNextIndicator = 0;   // 1 bit
    uint8_t mSectionNumber = 0;          // 8 bits
    uint8_t mLastSectionNumber = 0;      // 8 bits
    uint8_t mReserved2 = 0;              // 3 bits
    uint16_t mPcrPid = 0;                // 13 bits
    uint8_t mReserved3 = 0;              // 4 bits
    uint16_t mProgramInfoLength = 0;     // 12 bits
    std::vector<std::shared_ptr<PMTElementInfo>> mInfos;
};

class AdaptationFieldHeader final {
public:
    AdaptationFieldHeader() = default;

    ~AdaptationFieldHeader() = default;

    void encode(SimpleBuffer& rSb) const;

    void decode(SimpleBuffer& rSb);

    uint8_t mAdaptationFieldLength = 0;             // 8 bits
    uint8_t mAdaptationFieldExtensionFlag = 0;      // 1 bit
    uint8_t mTransportPrivateDataFlag = 0;          // 1 bit
    uint8_t mSplicingPointFlag = 0;                 // 1 bit
    uint8_t mOpcrFlag = 0;                          // 1 bit
    uint8_t mPcrFlag = 0;                           // 1 bit
    uint8_t mElementaryStreamPriorityIndicator = 0; // 1 bit
    uint8_t mRandomAccessIndicator = 0;             // 1 bit
    uint8_t mDiscontinuityIndicator = 0;            // 1 bit
};

class PESHeader final {
public:
    PESHeader() = default;

    ~PESHeader() = default;

    void encode(SimpleBuffer& rSb) const;

    void decode(SimpleBuffer& rSb);

    uint32_t mPacketStartCode = 0x000001; // 24 bits
    uint8_t mStreamId = 0;                // 8 bits
    uint16_t mPesPacketLength = 0;        // 16 bits
    uint8_t mOriginalOrCopy = 0;          // 1 bit
    uint8_t mCopyright = 0;               // 1 bit
    uint8_t mDataAlignmentIndicator = 0;  // 1 bit
    uint8_t mPesPriority = 0;             // 1 bit
    uint8_t mPesScramblingControl = 0;    // 2 bits
    uint8_t mMarkerBits = 0x02;           // 2 bits
    uint8_t mPesExtFlag = 0;              // 1 bit
    uint8_t mPesCrcFlag = 0;              // 1 bit
    uint8_t mAddCopyInfoFlag = 0;         // 1 bit
    uint8_t mDsmTrickModeFlag = 0;        // 1 bit
    uint8_t mEsRateFlag = 0;              // 1 bit
    uint8_t mEscrFlag = 0;                // 1 bit
    uint8_t mPtsDtsFlags = 0;             // 2 bits
    uint8_t mHeaderDataLength = 0;        // 8 bits
};

}
