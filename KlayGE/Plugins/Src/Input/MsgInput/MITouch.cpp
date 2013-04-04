/**
 * @file MIJoystick.cpp
 * @author Minmin Gong
 *
 * @section DESCRIPTION
 *
 * This source file is part of KlayGE
 * For the latest info, see http://www.klayge.org
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * You may alternatively use this source under the terms of
 * the KlayGE Proprietary License (KPL). You can obtained such a license
 * from http://www.klayge.org/licensing/.
 */

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/InputFactory.hpp>
#include <KlayGE/Window.hpp>

#include <windowsx.h>

#include <KlayGE/MsgInput/MInput.hpp>

namespace KlayGE
{
	MsgInputTouch::MsgInputTouch()
	{
		touch_down_state_.fill(false);
	}
	
	const std::wstring& MsgInputTouch::Name() const
	{
		static std::wstring const name(L"MsgInput Touch");
		return name;
	}

	void MsgInputTouch::OnTouch(Window const & wnd, uint64_t lparam, uint32_t wparam)
	{
#if (_WIN32_WINNT >= 0x0601 /*_WIN32_WINNT_WIN7*/)
		uint32_t num_inputs = LOWORD(wparam);
		std::vector<TOUCHINPUT> inputs(num_inputs);

		HTOUCHINPUT hti = reinterpret_cast<HTOUCHINPUT>(lparam);
		if (::GetTouchInputInfo(hti, num_inputs, &inputs[0], sizeof(inputs[0])))
		{
			typedef KLAYGE_DECLTYPE(inputs) InputsType;
			KLAYGE_FOREACH(InputsType::const_reference ti, inputs)
			{
				POINT pt = { TOUCH_COORD_TO_PIXEL(ti.x), TOUCH_COORD_TO_PIXEL(ti.y) };
				::MapWindowPoints(NULL, wnd.HWnd(), &pt, 1);
				touch_coord_state_[ti.dwID] = int2(pt.x, pt.y);
				touch_down_state_[ti.dwID] = (ti.dwFlags & (TOUCHEVENTF_MOVE | TOUCHEVENTF_DOWN)) ? true : false;
			}

			::CloseTouchInputHandle(hti);
		}
#else
		UNREF_PARAM(wnd);
		UNREF_PARAM(lparam);
		UNREF_PARAM(wparam);
#endif
	}

	void MsgInputTouch::OnPointerDown(Window const & wnd, uint64_t lparam, uint32_t wparam)
	{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		for (uint32_t f = 0; f < 5; ++ f)
		{
			if (IS_POINTER_FLAG_SET_WPARAM(wparam, static_cast<uint32_t>(POINTER_MESSAGE_FLAG_FIRSTBUTTON << f)))
			{
				POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
				::ScreenToClient(wnd.HWnd(), &pt);
				touch_coord_state_[f] = int2(pt.x, pt.y);
				touch_down_state_[f] = true;
			}
		}
#else
		UNREF_PARAM(wnd);
		UNREF_PARAM(lparam);
		UNREF_PARAM(wparam);
#endif
	}

	void MsgInputTouch::OnPointerUp(Window const & wnd, uint64_t lparam, uint32_t wparam)
	{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		for (uint32_t f = 0; f < 5; ++ f)
		{
			if (!IS_POINTER_FLAG_SET_WPARAM(wparam, static_cast<uint32_t>(POINTER_MESSAGE_FLAG_FIRSTBUTTON << f)))
			{
				POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
				::ScreenToClient(wnd.HWnd(), &pt);
				touch_coord_state_[f] = int2(pt.x, pt.y);
				touch_down_state_[f] = false;
			}
		}
#else
		UNREF_PARAM(wnd);
		UNREF_PARAM(lparam);
		UNREF_PARAM(wparam);
#endif
	}

	void MsgInputTouch::OnPointerUpdate(Window const & wnd, uint64_t lparam, uint32_t wparam)
	{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
		for (uint32_t f = 0; f < 5; ++ f)
		{
			if (IS_POINTER_FLAG_SET_WPARAM(wparam, static_cast<uint32_t>(POINTER_MESSAGE_FLAG_FIRSTBUTTON << f)))
			{
				POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
				::ScreenToClient(wnd.HWnd(), &pt);
				touch_coord_state_[f] = int2(pt.x, pt.y);
				touch_down_state_[f] = true;
			}
		}
#else
		UNREF_PARAM(wnd);
		UNREF_PARAM(lparam);
		UNREF_PARAM(wparam);
#endif
	}

	void MsgInputTouch::UpdateInputs()
	{
		index_ = !index_;
		touch_coords_[index_] = touch_coord_state_;
		touch_downs_[index_] = touch_down_state_;
		num_available_touch_ = 0;
		typedef KLAYGE_DECLTYPE(touch_down_state_) TDSType;
		KLAYGE_FOREACH(TDSType::const_reference tds, touch_down_state_)
		{
			num_available_touch_ += tds;
		}

		has_gesture_ = false;
		action_param_->move_vec = float2(0.0f, 0.0f);
		curr_gesture_(static_cast<float>(timer_.elapsed()));
		timer_.restart();
	}
}
