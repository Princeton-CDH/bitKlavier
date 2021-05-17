/*
 ==============================================================================
 
 BKWavReader.h
 Created: 17 May 2021 12:48:08pm
 Author:  Matthew Wang
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// Copy of juce_WavAudioFormat with some changes. Unfortunately need to copy pretty much everything
// since many of the classes only exist in the cpp file of JUCE and aren't normally accessible

class BKWavAudioFormat  : public AudioFormat
{
public:
    //==============================================================================
    /** Creates a format object. */
    BKWavAudioFormat();
    
    /** Destructor. */
    ~BKWavAudioFormat() override;
    
    //==============================================================================
    // BWAV chunk properties:
    
    static const char* const bwavDescription;       /**< Metadata property name used in BWAV chunks. */
    static const char* const bwavOriginator;        /**< Metadata property name used in BWAV chunks. */
    static const char* const bwavOriginatorRef;     /**< Metadata property name used in BWAV chunks. */
    static const char* const bwavOriginationDate;   /**< Metadata property name used in BWAV chunks. The format should be: yyyy-mm-dd */
    static const char* const bwavOriginationTime;   /**< Metadata property name used in BWAV chunks. The format should be: format is: hh-mm-ss */
    static const char* const bwavCodingHistory;     /**< Metadata property name used in BWAV chunks. */
    
    /** Metadata property name used in BWAV chunks.
     This is the number of samples from the start of an edit that the
     file is supposed to begin at. Seems like an obvious mistake to
     only allow a file to occur in an edit once, but that's the way
     it is..
     
     @see AudioFormatReader::metadataValues, createWriterFor
     */
    static const char* const bwavTimeReference;
    
    /** Utility function to fill out the appropriate metadata for a BWAV file.
     
     This just makes it easier than using the property names directly, and it
     fills out the time and date in the right format.
     */
    static StringPairArray createBWAVMetadata (const String& description,
                                               const String& originator,
                                               const String& originatorRef,
                                               Time dateAndTime,
                                               int64 timeReferenceSamples,
                                               const String& codingHistory);
    
    //==============================================================================
    // 'acid' chunk properties:
    
    static const char* const acidOneShot;           /**< Metadata property name used in acid chunks. */
    static const char* const acidRootSet;           /**< Metadata property name used in acid chunks. */
    static const char* const acidStretch;           /**< Metadata property name used in acid chunks. */
    static const char* const acidDiskBased;         /**< Metadata property name used in acid chunks. */
    static const char* const acidizerFlag;          /**< Metadata property name used in acid chunks. */
    static const char* const acidRootNote;          /**< Metadata property name used in acid chunks. */
    static const char* const acidBeats;             /**< Metadata property name used in acid chunks. */
    static const char* const acidDenominator;       /**< Metadata property name used in acid chunks. */
    static const char* const acidNumerator;         /**< Metadata property name used in acid chunks. */
    static const char* const acidTempo;             /**< Metadata property name used in acid chunks. */
    
    //==============================================================================
    // INFO chunk properties:
    
    static const char* const riffInfoArchivalLocation;      /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoArtist;                /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoBaseURL;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCinematographer;       /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoComment;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoComment2;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoComments;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCommissioned;          /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCopyright;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCostumeDesigner;       /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCountry;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoCropped;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDateCreated;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDateTimeOriginal;      /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDefaultAudioStream;    /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDimension;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDirectory;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDistributedBy;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoDotsPerInch;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoEditedBy;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoEighthLanguage;        /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoEncodedBy;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoEndTimecode;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoEngineer;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoFifthLanguage;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoFirstLanguage;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoFourthLanguage;        /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoGenre;                 /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoKeywords;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLanguage;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLength;                /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLightness;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLocation;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLogoIconURL;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoLogoURL;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMedium;                /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMoreInfoBannerImage;   /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMoreInfoBannerURL;     /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMoreInfoText;          /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMoreInfoURL;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoMusicBy;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoNinthLanguage;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoNumberOfParts;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoOrganisation;          /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoPart;                  /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoProducedBy;            /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoProductName;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoProductionDesigner;    /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoProductionStudio;      /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoRate;                  /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoRated;                 /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoRating;                /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoRippedBy;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSecondaryGenre;        /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSecondLanguage;        /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSeventhLanguage;       /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSharpness;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSixthLanguage;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSoftware;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSoundSchemeTitle;      /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSource;                /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSourceFrom;            /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoStarring_ISTR;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoStarring_STAR;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoStartTimecode;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoStatistics;            /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoSubject;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTapeName;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTechnician;            /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoThirdLanguage;         /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTimeCode;              /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTitle;                 /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTrackNo;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoTrackNumber;           /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoURL;                   /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoVegasVersionMajor;     /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoVegasVersionMinor;     /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoVersion;               /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoWatermarkURL;          /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoWrittenBy;             /**< Metadata property name used in INFO chunks. */
    static const char* const riffInfoYear;                  /**< Metadata property name used in INFO chunks. */
    
    //==============================================================================
    /** Metadata property name used when reading an ISRC code from an AXML chunk. */
    static const char* const ISRC;
    
    /** Metadata property name used when reading a WAV file with a Tracktion chunk. */
    static const char* const tracktionLoopInfo;
    
    //==============================================================================
    Array<int> getPossibleSampleRates() override;
    Array<int> getPossibleBitDepths() override;
    bool canDoStereo() override;
    bool canDoMono() override;
    bool isChannelLayoutSupported (const AudioChannelSet& channelSet) override;
    
    //==============================================================================
    AudioFormatReader* createReaderFor (InputStream* sourceStream,
                                        bool deleteStreamIfOpeningFails) override;
    
    MemoryMappedAudioFormatReader* createMemoryMappedReader (const File&)      override;
    MemoryMappedAudioFormatReader* createMemoryMappedReader (FileInputStream*) override;
    
    AudioFormatWriter* createWriterFor (OutputStream* streamToWriteTo,
                                        double sampleRateToUse,
                                        unsigned int numberOfChannels,
                                        int bitsPerSample,
                                        const StringPairArray& metadataValues,
                                        int qualityOptionIndex) override;
    
    AudioFormatWriter* createWriterFor (OutputStream* streamToWriteTo,
                                        double sampleRateToUse,
                                        const AudioChannelSet& channelLayout,
                                        int bitsPerSample,
                                        const StringPairArray& metadataValues,
                                        int qualityOptionIndex) override;
    using AudioFormat::createWriterFor;
    
    //==============================================================================
    /** Utility function to replace the metadata in a wav file with a new set of values.
     
     If possible, this cheats by overwriting just the metadata region of the file, rather
     than by copying the whole file again.
     */
    bool replaceMetadataInFile (const File& wavFile, const StringPairArray& newMetadata);
    
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKWavAudioFormat)
};

//==============================================================================
class BKWavAudioFormatReader  : public AudioFormatReader
{
public:
    BKWavAudioFormatReader (InputStream* in);
    
    //==============================================================================
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override;
    
    static void copySampleData (unsigned int numBitsPerSample, const bool floatingPointData,
                                int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numberOfChannels, int numSamples) noexcept;
    
    static void copySampleData (unsigned int numBitsPerSample, const bool floatingPointData,
                                float* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numberOfChannels, int numSamples) noexcept;
    
    //==============================================================================
    AudioChannelSet getChannelLayout() override;
    
    static AudioChannelSet getChannelLayoutFromMask (int dwChannelMask, size_t totalNumChannels);
    
    int64 bwavChunkStart = 0, bwavSize = 0;
    int64 dataChunkStart = 0, dataLength = 0;
    int bytesPerFrame = 0;
    bool isRF64 = false;
    bool isSubformatOggVorbis = false;
    
    AudioChannelSet channelLayout;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKWavAudioFormatReader)
};

//==============================================================================
class BKWavAudioFormatWriter  : public AudioFormatWriter
{
public:
    BKWavAudioFormatWriter (OutputStream* const out, const double rate,
                            const AudioChannelSet& channelLayoutToUse, const unsigned int bits,
                            const StringPairArray& metadataValues);
    
    ~BKWavAudioFormatWriter() override;
    
    //==============================================================================
    bool write (const int** data, int numSamples) override;
    
    bool flush() override;
    
private:
    MemoryBlock tempBlock, bwavChunk, axmlChunk, smplChunk, instChunk, cueChunk, listChunk, listInfoChunk, acidChunk, trckChunk;
    uint64 lengthInSamples = 0, bytesWritten = 0;
    int64 headerPosition = 0;
    bool writeFailed = false;
    
    void writeHeader();
    
    static size_t chunkSize (const MemoryBlock& data) noexcept;
    
    void writeChunkHeader (int chunkType, int size) const;
    
    void writeChunk (const MemoryBlock& data, int chunkType, int size = 0) const;
    
    static int getChannelMaskFromChannelLayout (const AudioChannelSet& layout);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKWavAudioFormatWriter)
};

//==============================================================================
class BKMemoryMappedWavReader   : public MemoryMappedAudioFormatReader
{
public:
    BKMemoryMappedWavReader (const File& wavFile, const BKWavAudioFormatReader& reader);
    
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override;
    
    bool readSamples (float** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples);
    
    void getSample (int64 sample, float* result) const noexcept override;
    
    void readMaxLevels (int64 startSampleInFile, int64 numSamples, Range<float>* results, int numChannelsToRead) override;
    
    using AudioFormatReader::readMaxLevels;
    
private:
    template <typename SampleType>
    void scanMinAndMax (int64 startSampleInFile, int64 numSamples, Range<float>* results, int numChannelsToRead);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKMemoryMappedWavReader)
};
