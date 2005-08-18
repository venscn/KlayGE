// RenderableHelper.cpp
// KlayGE 一些常用的可渲染对象 实现文件
// Ver 2.7.1
// 版权所有(C) 龚敏敏, 2005
// Homepage: http://klayge.sourceforge.net
//
// 2.7.1
// 增加了RenderableHelper基类 (2005.7.10)
//
// 2.6.0
// 增加了RenderableSkyBox (2005.5.26)
//
// 2.5.0
// 增加了RenderablePoint，RenderableLine和RenderableTriangle (2005.4.13)
//
// 2.4.0
// 初次建立 (2005.3.22)
//
// 修改记录
//////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/VertexBuffer.hpp>
#include <KlayGE/RenderEffect.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/Sampler.hpp>
#include <KlayGE/App3D.hpp>

#include <KlayGE/RenderableHelper.hpp>

namespace KlayGE
{
	RenderEffectPtr RenderableHelper::GetRenderEffect() const
	{
		return effect_;
	}

	VertexBufferPtr RenderableHelper::GetVertexBuffer() const
	{
		return vb_;
	}

	Box RenderableHelper::GetBound() const
	{
		return box_;
	}


	RenderablePoint::RenderablePoint(Vector3 const & v)
	{
		effect_ = Context::Instance().RenderFactoryInstance().LoadEffect("RenderableHelper.fx");
		effect_->SetTechnique("PointTec");

		vb_.reset(new VertexBuffer(VertexBuffer::BT_PointList));
		vb_->AddVertexStream(VST_Positions, sizeof(float), 3, true);
		vb_->GetVertexStream(VST_Positions)->Assign(&v, 1);

		box_ = MathLib::ComputeBoundingBox<float>(&v, &v + 1);
	}

	std::wstring const & RenderablePoint::Name() const
	{
		static std::wstring const name(L"Point");
		return name;
	}


	RenderableLine::RenderableLine(Vector3 const & v0, Vector3 const & v1)
	{
		effect_ = Context::Instance().RenderFactoryInstance().LoadEffect("RenderableHelper.fx");
		effect_->SetTechnique("LineTec");

		Vector3 xyzs[] =
		{
			v0, v1
		};

		vb_.reset(new VertexBuffer(VertexBuffer::BT_LineList));
		vb_->AddVertexStream(VST_Positions, sizeof(float), 3, true);
		vb_->GetVertexStream(VST_Positions)->Assign(xyzs, sizeof(xyzs) / sizeof(xyzs[0]));

		box_ = MathLib::ComputeBoundingBox<float>(&xyzs[0], &xyzs[0] + sizeof(xyzs) / sizeof(xyzs[0]));
	}

	std::wstring const & RenderableLine::Name() const
	{
		static std::wstring const name(L"Line");
		return name;
	}


	RenderableTriangle::RenderableTriangle(Vector3 const & v0, Vector3 const & v1, Vector3 const & v2)
	{
		effect_ = Context::Instance().RenderFactoryInstance().LoadEffect("RenderableHelper.fx");
		effect_->SetTechnique("TriangleTec");

		Vector3 xyzs[] =
		{
			v0, v1, v2
		};

		vb_.reset(new VertexBuffer(VertexBuffer::BT_TriangleList));
		vb_->AddVertexStream(VST_Positions, sizeof(float), 3, true);
		vb_->GetVertexStream(VST_Positions)->Assign(xyzs, sizeof(xyzs) / sizeof(xyzs[0]));

		box_ = MathLib::ComputeBoundingBox<float>(&xyzs[0], &xyzs[0] + sizeof(xyzs) / sizeof(xyzs[0]));
	}

	std::wstring const & RenderableTriangle::Name() const
	{
		static std::wstring const name(L"Triangle");
		return name;
	}


	RenderableBox::RenderableBox(Box const & box)
	{
		box_ = box;

		effect_ = Context::Instance().RenderFactoryInstance().LoadEffect("RenderableHelper.fx");
		effect_->SetTechnique("BoxTec");

		Vector3 xyzs[] =
		{
			box[0], box[1], box[2], box[3], box[4], box[5], box[6], box[7]
		};

		uint16_t indices[] =
		{
			0, 1, 2, 2, 3, 0,
			7, 6, 5, 5, 4, 7,
			4, 0, 3, 3, 7, 4,
			4, 5, 1, 1, 0, 4,
			1, 5, 6, 6, 2, 1,
			3, 2, 6, 6, 7, 3,
		};

		vb_.reset(new VertexBuffer(VertexBuffer::BT_TriangleList));
		vb_->AddVertexStream(VST_Positions, sizeof(float), 3, true);
		vb_->GetVertexStream(VST_Positions)->Assign(xyzs, sizeof(xyzs) / sizeof(xyzs[0]));
		
		vb_->AddIndexStream(true);
		vb_->GetIndexStream()->Assign(indices, sizeof(indices) / sizeof(indices[0]));
	}

	std::wstring const & RenderableBox::Name() const
	{
		static std::wstring const name(L"Box");
		return name;
	}


	RenderableSkyBox::RenderableSkyBox()
		: cube_sampler_(new Sampler)
	{
		effect_ = Context::Instance().RenderFactoryInstance().LoadEffect("RenderableHelper.fx");
		effect_->SetTechnique("SkyBoxTec");

		Vector3 xyzs[] =
		{
			Vector3(1.0f, 1.0f, 1.0f),
			Vector3(1.0f, -1.0f, 1.0f),
			Vector3(-1.0f, -1.0f, 1.0f),
			Vector3(-1.0f, 1.0f, 1.0f),
		};

		uint16_t indices[] =
		{
			0, 1, 2, 2, 3, 0,
		};

		vb_.reset(new VertexBuffer(VertexBuffer::BT_TriangleList));
		vb_->AddVertexStream(VST_Positions, sizeof(float), 3, true);
		vb_->GetVertexStream(VST_Positions)->Assign(xyzs, sizeof(xyzs) / sizeof(xyzs[0]));

		vb_->AddIndexStream(true);
		vb_->GetIndexStream()->Assign(indices, sizeof(indices) / sizeof(uint16_t));

		box_ = MathLib::ComputeBoundingBox<float>(&xyzs[0], &xyzs[4]);

		cube_sampler_->Filtering(Sampler::TFO_Bilinear);
		cube_sampler_->AddressingMode(Sampler::TAT_Addr_U, Sampler::TAM_Clamp);
		cube_sampler_->AddressingMode(Sampler::TAT_Addr_V, Sampler::TAM_Clamp);
		cube_sampler_->AddressingMode(Sampler::TAT_Addr_W, Sampler::TAM_Clamp);
		*(effect_->ParameterByName("skybox_cubeMapSampler")) = cube_sampler_;
	}

	void RenderableSkyBox::CubeMap(TexturePtr const & cube)
	{
		cube_sampler_->SetTexture(cube);
	}

	void RenderableSkyBox::OnRenderBegin()
	{
		App3DFramework const & app = Context::Instance().AppInstance();
		Camera const & camera = app.ActiveCamera();

		Matrix4 rot_view = camera.ViewMatrix();
		rot_view(3, 0) = 0;
		rot_view(3, 1) = 0;
		rot_view(3, 2) = 0;
		*(effect_->ParameterByName("inv_mvp")) = MathLib::Inverse(rot_view * camera.ProjMatrix());
	}

	bool RenderableSkyBox::CanBeCulled() const
	{
		return false;
	}

	std::wstring const & RenderableSkyBox::Name() const
	{
		static std::wstring const name(L"SkyBox");
		return name;
	}
}
