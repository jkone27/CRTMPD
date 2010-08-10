/* 
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAS_MEDIA_MP4
#include "mediaformats/mp4/mp4document.h"
#include "mediaformats/mp4/baseatom.h"
#include "mediaformats/mp4/atomudta.h"
#include "mediaformats/mp4/atomftyp.h"
#include "mediaformats/mp4/atommoov.h"
#include "mediaformats/mp4/atommvhd.h"
#include "mediaformats/mp4/atomtrak.h"
#include "mediaformats/mp4/atomtkhd.h"
#include "mediaformats/mp4/atommdia.h"
#include "mediaformats/mp4/atommdhd.h"
#include "mediaformats/mp4/atomhdlr.h"
#include "mediaformats/mp4/atomminf.h"
#include "mediaformats/mp4/atomsmhd.h"
#include "mediaformats/mp4/atomdinf.h"
#include "mediaformats/mp4/atomstbl.h"
#include "mediaformats/mp4/atomvmhd.h"
#include "mediaformats/mp4/atomdref.h"
#include "mediaformats/mp4/atomstsd.h"
#include "mediaformats/mp4/atomstts.h"
#include "mediaformats/mp4/atomstsc.h"
#include "mediaformats/mp4/atomstsz.h"
#include "mediaformats/mp4/atomstco.h"
#include "mediaformats/mp4/atomctts.h"
#include "mediaformats/mp4/atomstss.h"
#include "mediaformats/mp4/atomurl.h"
#include "mediaformats/mp4/atommp4a.h"
#include "mediaformats/mp4/atomavc1.h"
#include "mediaformats/mp4/atomesds.h"
#include "mediaformats/mp4/atomavcc.h"
#include "mediaformats/mp4/atomwave.h"
#include "mediaformats/mp4/atommeta.h"
#include "mediaformats/mp4/atomilst.h"
#include "mediaformats/mp4/atomdata.h"
#include "mediaformats/mp4/atomnull.h"
#include "mediaformats/mp4/atommetafield.h"
#include "mediaformats/mp4/ignoredatom.h"
#include "mediaformats/mp4/atomco64.h"

//TODO: See how the things are impemented inside mp4v2. Good source
//for looking at the avcC atom format for exampl

MP4Document::MP4Document(Variant &metadata)
: BaseMediaDocument(metadata) {
	_pFTYP = NULL;
	_pMOOV = NULL;
}

MP4Document::~MP4Document() {

	FOR_VECTOR_ITERATOR(BaseAtom *, _allAtoms, i) {
		delete VECTOR_VAL(i);
	}
	_allAtoms.clear();
}

#ifdef HAS_MMAP

MmapFile &MP4Document::GetMediaFile() {
	return _mediaFile;
}
#else

File &MP4Document::GetMediaFile() {
	return _mediaFile;
}
#endif /* HAS_MMAP */

void MP4Document::AddAtom(BaseAtom *pAtom) {
	ADD_VECTOR_END(_allAtoms, pAtom);
}

BaseAtom * MP4Document::ReadAtom(BaseAtom *pParentAtom) {
	uint64_t currentPos = _mediaFile.Cursor();

	uint64_t size = 0;
	uint32_t type = 0;
	BaseAtom *pAtom = NULL;

	uint32_t size32 = 0;
	if (!_mediaFile.ReadUI32(& size32)) {
		FATAL("Unable to read atom size");
		return NULL;
	}

	size = size32;

	if (size == 0) {
		pAtom = new AtomNULL(this, type, size, currentPos);
		pAtom->SetParentAtom(pParentAtom);
		return pAtom;
	}

	if (!_mediaFile.ReadUI32(&type)) {
		FATAL("Unable to read atom type");
		return NULL;
	}

	if (size == 1) {
		if (!_mediaFile.ReadUI64(& size)) {
			FATAL("Unable to read atom size");
			return NULL;
		}

		if (size == 0) {
			pAtom = new AtomNULL(this, type, size, currentPos);
			pAtom->SetParentAtom(pParentAtom);
			return pAtom;
		}
	}

	switch (type) {
		case A_FTYP:
			pAtom = new AtomFTYP(this, type, size, currentPos);
			break;
		case A_MOOV:
			pAtom = new AtomMOOV(this, type, size, currentPos);
			break;
		case A_MVHD:
			pAtom = new AtomMVHD(this, type, size, currentPos);
			break;
		case A_TRAK:
			pAtom = new AtomTRAK(this, type, size, currentPos);
			break;
		case A_TKHD:
			pAtom = new AtomTKHD(this, type, size, currentPos);
			break;
		case A_MDIA:
			pAtom = new AtomMDIA(this, type, size, currentPos);
			break;
		case A_MDHD:
			pAtom = new AtomMDHD(this, type, size, currentPos);
			break;
		case A_HDLR:
			pAtom = new AtomHDLR(this, type, size, currentPos);
			break;
		case A_MINF:
			pAtom = new AtomMINF(this, type, size, currentPos);
			break;
		case A_SMHD:
			pAtom = new AtomSMHD(this, type, size, currentPos);
			break;
		case A_DINF:
			pAtom = new AtomDINF(this, type, size, currentPos);
			break;
		case A_STBL:
			pAtom = new AtomSTBL(this, type, size, currentPos);
			break;
		case A_VMHD:
			pAtom = new AtomVMHD(this, type, size, currentPos);
			break;
		case A_DREF:
			pAtom = new AtomDREF(this, type, size, currentPos);
			break;
		case A_STSD:
			pAtom = new AtomSTSD(this, type, size, currentPos);
			break;
		case A_STTS:
			pAtom = new AtomSTTS(this, type, size, currentPos);
			break;
		case A_STSC:
			pAtom = new AtomSTSC(this, type, size, currentPos);
			break;
		case A_STSZ:
			pAtom = new AtomSTSZ(this, type, size, currentPos);
			break;
		case A_STCO:
			pAtom = new AtomSTCO(this, type, size, currentPos);
			break;
		case A_CTTS:
			pAtom = new AtomCTTS(this, type, size, currentPos);
			break;
		case A_STSS:
			pAtom = new AtomSTSS(this, type, size, currentPos);
			break;
		case A_URL:
			pAtom = new AtomURL(this, type, size, currentPos);
			break;
		case A_MP4A:
			pAtom = new AtomMP4A(this, type, size, currentPos);
			break;
		case A_AVC1:
			pAtom = new AtomAVC1(this, type, size, currentPos);
			break;
		case A_ESDS:
			pAtom = new AtomESDS(this, type, size, currentPos);
			break;
		case A_AVCC:
			pAtom = new AtomAVCC(this, type, size, currentPos);
			break;
		case A_UDTA:
			pAtom = new AtomUDTA(this, type, size, currentPos);
			break;
		case A_WAVE:
			pAtom = new AtomWAVE(this, type, size, currentPos);
			break;
		case A_META:
			pAtom = new AtomMETA(this, type, size, currentPos);
			break;
		case A_NULL:
			pAtom = new AtomNULL(this, type, size, currentPos);
			break;
		case A_ILST:
			pAtom = new AtomILST(this, type, size, currentPos);
			break;
		case A_DATA:
			pAtom = new AtomDATA(this, type, size, currentPos);
			break;
		case A_CO64:
			pAtom = new AtomCO64(this, type, size, currentPos);
			break;
		case A__COM:
		case A_NAME:
		case A_COVR:
		case A_AART:
		case A__WRT:
		case A__GRP:
		case A__LYR:
		case A__NAM:
		case A__TOO:
		case A__DAY:
		case A__CMT:
		case A__CPY:
		case A__DES:
		case A__ART:
		case A__ALB:
		case A_TRKN:
		case A_CPIL:
		case A_PGAP:
		case A_TMPO:
		case A_GNRE:
		case A_DISK:
		case A__GEN:
		case A_DESC:
		case A_TVSH:
		case A_TVEN:
		case A_TVSN:
		case A_TVES:
			pAtom = new AtomMetaField(this, type, size, currentPos);
			break;
		default:
		{
			//            FATAL("Atom type %s(%u) not yet implemented. Position: 0x%x",
			//                    STR(U32TOS(type)), type, (uint32_t) _mediaFile.Cursor());
			//            return NULL;
			pAtom = new IgnoredAtom(this, type, size, currentPos);
			break;
		}
	}

	pAtom->SetParentAtom(pParentAtom);

	if (!pAtom->Read()) {
		FATAL("Unable to read atom type %s", STR(U32TOS(type)));
		return NULL;
	}

	//    FINEST("END reading atom %s at 0x%x with size 0x%x",
	//            STR(U32TOS(type)), currentPos, size);

	if (currentPos + pAtom->GetSize() != _mediaFile.Cursor()) {
		FATAL("atom start: %llu; Atom size: %llu; currentPos: %llu",
				currentPos, pAtom->GetSize(), _mediaFile.Cursor());
		return NULL;
	}
	return pAtom;
}

bool MP4Document::ParseDocument() {

	if (!_mediaFile.SeekBegin()) {
		FATAL("Unable to seek to the begining of file");
		return false;
	}


	while (!_mediaFile.Failed()) {
		uint64_t currentPos = _mediaFile.Cursor();
		if (currentPos == _mediaFile.Size()) {
			FINEST("\n%s", STR(Hierarchy()));
			return true;
		}
		BaseAtom *pAtom = ReadAtom(NULL);
		if (pAtom == NULL) {
			FATAL("Unable to read atom");
			return false;
		}
		if (!pAtom->IsIgnored()) {
			switch (pAtom->GetTypeNumeric()) {
				case A_FTYP:
					_pFTYP = (AtomFTYP *) pAtom;
					break;
				case A_MOOV:
					_pMOOV = (AtomMOOV *) pAtom;
					break;
				default:
				{
					FATAL("Invalid atom %s", STR(pAtom->GetTypeString()));
					return false;
				}
			}
		}
		ADD_VECTOR_END(_topAtoms, pAtom);
	}

	return true;
}

bool MP4Document::BuildFrames() {
	_frames.clear();

	AtomTRAK *pTrack;
	AtomAVCC *pAVCC = NULL;
	if ((pTrack = GetTRAK(false)) != NULL) {
		pAVCC = (AtomAVCC *) pTrack->GetPath(6, A_MDIA, A_MINF,
				A_STBL, A_STSD, A_AVC1, A_AVCC);
	}

	AtomESDS *pESDS = NULL;
	if ((pTrack = GetTRAK(true)) != NULL) {
		pESDS = (AtomESDS *) pTrack->GetPath(6, A_MDIA, A_MINF,
				A_STBL, A_STSD, A_MP4A, A_ESDS);
		if (pESDS == NULL) {
			pESDS = (AtomESDS *) pTrack->GetPath(7, A_MDIA, A_MINF,
					A_STBL, A_STSD, A_MP4A, A_WAVE, A_ESDS);
		}
	}

	if (pESDS != NULL) {
		//Build audio frames
		if (!BuildFrames(true)) {
			FATAL("Unable to build audio frames");
			return false;
		}
	}

	if (pAVCC != NULL) {
		//Build video frames
		if (!BuildFrames(false)) {
			FATAL("Unable to build video frames");
			return false;
		}
	}

	//sort the frames
	sort(_frames.begin(), _frames.end(), CompareFrames);

	//add binary audio header
	MediaFrame audioHeader = {0};
	if (pESDS != NULL) {
		audioHeader.type = MEDIAFRAME_TYPE_AUDIO;
		audioHeader.isBinaryHeader = true;
		audioHeader.isKeyFrame = true;
		audioHeader.length = pESDS->GetExtraDataLength();
		audioHeader.absoluteTime = 0;
		audioHeader.start = pESDS->GetExtraDataStart();
		audioHeader.deltaTime = 0;
		audioHeader.compositionOffset = 0;
		FINEST("Start: %llu (%llx); Length: %llu (%llx);", audioHeader.start,
				audioHeader.start, audioHeader.length, audioHeader.length);
	}
	//ADD_VECTOR_BEGIN(_frames, audioHeader);

	//add binary video header
	MediaFrame videoHeader = {0};
	if (pAVCC != NULL) {
		videoHeader.type = MEDIAFRAME_TYPE_VIDEO;
		videoHeader.isBinaryHeader = true;
		videoHeader.isKeyFrame = true;
		videoHeader.length = pAVCC->GetExtraDataLength();
		videoHeader.absoluteTime = 0;
		videoHeader.start = pAVCC->GetExtraDataStart();
		videoHeader.deltaTime = 0;
		videoHeader.compositionOffset = 0;
		//    FINEST("Start: %u (%x); Length: %u (%x);", videoHeader.start,
		//            videoHeader.start, videoHeader.length,videoHeader.length);
		//ADD_VECTOR_BEGIN(_frames, videoHeader);
	}

	if (pESDS != NULL) {
		ADD_VECTOR_BEGIN(_frames, audioHeader);
	} else {
		WARN("No sound track found");
	}

	if (pAVCC != NULL) {
		ADD_VECTOR_BEGIN(_frames, videoHeader);
	} else {
		WARN("No video track found");
	}

	return true;
}

bool MP4Document::BuildFrames(bool audio) {
	//1. Get the track
	AtomTRAK *pTrack = GetTRAK(audio);
	if (pTrack == NULL) {
		FATAL("no %s track", audio ? "Audio" : "Video");
		return false;
	}

	//2. Get the atom containing sample sizes
	AtomSTSZ *pSTSZ = (AtomSTSZ *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_STSZ);
	if (pSTSZ == NULL) {
		FATAL("no STSZ");
		return false;
	}

	//3. Get the atom containing the chuncks offests
	AtomCO64 *pCO64 = NULL;
	AtomSTCO *pSTCO = (AtomSTCO *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_STCO);
	if (pSTCO == NULL) {
		pCO64 = (AtomCO64 *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL, A_CO64);
		if (pCO64 == NULL) {
			FATAL("no CO64");
			return false;
		}
	}

	//4. Get the atom containing the distribution of samples per corresponding
	//chunks
	AtomSTSC *pSTSC = (AtomSTSC *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_STSC);
	if (pSTSC == NULL) {
		FATAL("no STSC");
		return false;
	}

	//5. Get the atom containing the delta time of each sample
	AtomSTTS *pSTTS = (AtomSTTS *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_STTS);
	if (pSTTS == NULL) {
		FATAL("no STTS");
		return false;
	}

	//6. Get the atom containing the time scale of each delta time
	AtomMDHD *pMDHD = (AtomMDHD *) pTrack->GetPath(2, A_MDIA, A_MDHD);
	if (pMDHD == NULL) {
		FATAL("no MDHD");
		return false;
	}

	//7. Get the table containing the samples marked as key frames. It can be null
	//It can be null
	AtomSTSS *pSTSS = (AtomSTSS *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_STSS);

	//8. Get the composition timestamps
	AtomCTTS *pCTSS = (AtomCTTS *) pTrack->GetPath(4, A_MDIA, A_MINF, A_STBL,
			A_CTTS);

	//8. Extract normalized information from the atoms retrived above
	vector<uint64_t> sampleSize = pSTSZ->GetEntries();
	if (audio)
		_audioSamplesCount = sampleSize.size();
	else
		_videoSamplesCount = sampleSize.size();
	vector<uint32_t> sampleDeltaTime = pSTTS->GetEntries();
	vector<uint64_t> chunckOffsets;
	if (pSTCO != NULL)
		chunckOffsets = pSTCO->GetEntries();
	else
		chunckOffsets = pCO64->GetEntries();
	vector<uint32_t> sample2Chunk = pSTSC->GetEntries(chunckOffsets.size());
	vector<uint32_t> keyFrames;
	if (pSTSS != NULL)
		keyFrames = pSTSS->GetEntries();
	vector<int32_t> compositionOffsets;
	if (pCTSS != NULL) {
		compositionOffsets = pCTSS->GetEntries();
		if (sampleSize.size() != compositionOffsets.size()) {
			WARN("composition offsets count != samples count; compositionOffsets: %d; sampleSize.size: %d",
					compositionOffsets.size(),
					sampleSize.size());
			for (uint32_t i = compositionOffsets.size(); i < sampleSize.size(); i++)
				ADD_VECTOR_END(compositionOffsets, 0);
			WARN("composition offsets padded with 0. Now size is %d",
					compositionOffsets.size());
		}
	}
	INFO("audio: %d; keyFrames: %u; frames: %u; compositionOffsets: %u",
			audio, keyFrames.size(), sampleSize.size(), compositionOffsets.size());

	//    FINEST("sampleSize count: %d", sampleSize.size());
	//    FINEST("sampleDeltaTime count: %d", sampleDeltaTime.size());
	//    FINEST("chunckOffsets count: %d", chunckOffsets.size());
	//    FINEST("sample2Chunk count: %d", sample2Chunk.size());
	//    FINEST("keyFrames count: %d", keyFrames.size());

	uint32_t timeScale = pMDHD->GetTimeScale();
	uint32_t totalTime = 0;
	uint32_t localOffset = 0;
	uint32_t startIndex = _frames.size();

	for (uint32_t i = 0; i < sampleSize.size(); i++) {
		MediaFrame frame = {0};
		frame.start = chunckOffsets[sample2Chunk[i]] + localOffset;
		if (pCTSS != NULL) {
			double doubleVal = ((double) compositionOffsets[i] / (double) timeScale)*(double) 1000.00;
			frame.compositionOffset = (int32_t) doubleVal;
			//FINEST("frame.compositionOffset: %08x", frame.compositionOffset);
		} else {
			frame.compositionOffset = 0;
		}
		if (i <= sampleSize.size() - 2) {
			//not the last frame
			uint32_t currentChunck = sample2Chunk[i];
			uint32_t nextChunck = sample2Chunk[i + 1];


			if (currentChunck == nextChunck) {
				//not changing the chunk
				localOffset += (uint32_t) sampleSize[i];
			} else {
				//changing the chunck
				localOffset = 0;
			}

		} else {
			// we are near end, so we are on the same chunck
			localOffset += (uint32_t) sampleSize[i];
		}

		frame.length = sampleSize[i];
		frame.type = audio ? MEDIAFRAME_TYPE_AUDIO : MEDIAFRAME_TYPE_VIDEO;
		if (pSTSS == NULL) {
			//no kayframe info. All frames are considered key frames
			frame.isKeyFrame = true;
		} else {
			//default it to false, will set it later
			frame.isKeyFrame = false;
		}
		//frame.isKeyFrame = true;
		frame.deltaTime = ((double) sampleDeltaTime[i] / (double) timeScale)*(double) 1000.00;
		frame.absoluteTime = ((double) totalTime / (double) timeScale)*(double) 1000.00;
		frame.isBinaryHeader = false;
		totalTime += sampleDeltaTime[i];

		//FINEST("Frame: %s", STR(frame));
		ADD_VECTOR_END(_frames, frame);
	}

	FOR_VECTOR(keyFrames, i) {
		_frames[startIndex + keyFrames[i] - 1].isKeyFrame = true;
	}

	return true;
}

Variant MP4Document::GetRTMPMeta() {
	Variant result;

	AtomTRAK *pVideoTrack = GetTRAK(false);
	if (pVideoTrack != NULL) {
		AtomTKHD *pTKHD = (AtomTKHD *) pVideoTrack->GetPath(1, A_TKHD);
		if (pTKHD != NULL) {
			result["width"] = pTKHD->GetWidth();
			result["height"] = pTKHD->GetHeight();
		}
	}

	if (_pMOOV != NULL) {
		AtomILST *pILST = (AtomILST *) _pMOOV->GetPath(3, A_UDTA, A_META, A_ILST);

		if (pILST != NULL) {
			result["tags"] = pILST->GetVariant();
		} else {
			WARN("No ilst atom present");
		}
	}

	return result;
}

string MP4Document::Hierarchy() {
	string result = "";
	for (uint32_t i = 0; i < _topAtoms.size(); i++) {
		result += _topAtoms[i]->Hierarchy(1);
		if (i != _topAtoms.size() - 1)
			result += "\n";
	}
	return result;
}

AtomTRAK * MP4Document::GetTRAK(bool audio) {
	if (_pMOOV == NULL) {
		FATAL("Unable to find moov");
		return false;
	}
	vector<AtomTRAK *> tracks = _pMOOV->GetTracks();
	if (tracks.size() == 0) {
		FATAL("No tracks defined");
		return false;
	}
	for (uint32_t i = 0; i < tracks.size(); i++) {
		AtomHDLR *pHDLR = (AtomHDLR *) tracks[i]->GetPath(2, A_MDIA, A_HDLR);
		if (audio && (pHDLR->GetComponentSubType() == A_SOUN))
			return tracks[i];
		if ((!audio) && (pHDLR->GetComponentSubType() == A_VIDE))
			return tracks[i];
	}
	return NULL;
}


#endif /* HAS_MEDIA_MP4 */
