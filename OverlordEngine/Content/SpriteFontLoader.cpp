#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	//TODO_W5(L"Implement SpriteFontLoader >> Parse .fnt file")
	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	const int bmfAmount = 3;
	const char bmf[bmfAmount] = { 'B','M','F' };
	for (int i{ 0 }; i < bmfAmount; ++i)
	{
		if(bmf[i] != pReader->Read<char>())
		{
			Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
			return nullptr;
		}
	}


	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	const char version = 3;
	if (version != pReader->Read<char>())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	int blockId = pReader->Read<char>();
	int blockSize = pReader->Read<int>();
	int startPosition = pReader->GetBufferPosition();

	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = pReader->Read<short>();

	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pReader->SetBufferPosition(startPosition + 14);

	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();


	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	startPosition = pReader->GetBufferPosition();

	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	pReader->MoveBufferPosition(4);
	fontDesc.textureWidth = pReader->Read<short>();
	fontDesc.textureHeight = pReader->Read<short>();

	//Retrieve PageCount
	//> if pagecount > 1
	//	> Log Error (Only one texture per font is allowed!)
	short pageCount = pReader->Read<short>();
	if(pageCount > 1)
	{
		Logger::LogError(L"Only one texture per font is allowed!");
	}

	//Advance to Block2 (Move Reader)
	pReader->SetBufferPosition(startPosition + blockSize);

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	startPosition = pReader->GetBufferPosition();

	//Retrieve the PageName (BinaryReader::ReadNullString)
	std::wstring pageName = pReader->ReadNullString();

	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	fontDesc.pTexture = ContentManager::Load<TextureData>(loadInfo.assetFullPath.parent_path().append(pageName));

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pReader->Read<char>();
	blockSize = pReader->Read<int>();
	startPosition = pReader->GetBufferPosition();

	//Retrieve Character Count (see documentation)
	int charCount = blockSize / 20;

	//Create loop for Character Count, and:
	for(int i{}; i < charCount; ++i)
	{
		//	> Create instance of FontMetric (struct)
		FontMetric fontMetrics{};
		//	> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t characterId = static_cast<wchar_t>(pReader->Read<unsigned int>());
		//	> Set Character (CharacterId) [FontMetric::character]
		fontMetrics.character = characterId;
		//	> Retrieve Xposition (store Local)
		int x = pReader->Read<short>();
		//	> Retrieve Yposition (store Local)
		int y = pReader->Read<short>();
		//	> Retrieve & Set Width [FontMetric::width]
		fontMetrics.width = pReader->Read<short>();
		//	> Retrieve & Set Height [FontMetric::height]
		fontMetrics.height = pReader->Read<short>();
		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		fontMetrics.offsetX = pReader->Read<short>();
		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		fontMetrics.offsetY = pReader->Read<short>();
		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		fontMetrics.advanceX = pReader->Read<short>();
		//	> Retrieve & Set Page [FontMetric::page]
		fontMetrics.page = pReader->Read<char>();
		//	> Retrieve Channel (BITFIELD!!!) 
			//		> See documentation for BitField meaning [FontMetrix::channel]
		int channel = int(log2(pReader->Read<char>())); //Channel G & A are already correct, only B & R need to be swapped, but using log2 can make it a bit shorter
		if (channel == 0) 
			channel = 2;
		else if (channel == 2) 
			channel = 0;
		fontMetrics.channel = char(channel);

		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
		XMFLOAT2 texCoord = { x / float(fontDesc.textureWidth), y / float(fontDesc.textureHeight) };
		fontMetrics.texCoord = texCoord;

		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		fontDesc.metrics[characterId] = fontMetrics;

		//(loop restarts till all metrics are parsed)
	}


	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
