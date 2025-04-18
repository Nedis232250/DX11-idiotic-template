#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Shader {
	private:
		HRESULT hr;
		ID3D11Device* device;
		D3D_FEATURE_LEVEL level;
		ID3D11DeviceContext* ctx;
		ID3DBlob* error_blob;
		ID3DBlob* shader_blob;
	public:
		Shader(void) {};
		~Shader() {
			if (shader_blob) shader_blob->Release();
		}

		void Shader_device(ID3D11Device* dev, ID3D11DeviceContext* ctx_, D3D_FEATURE_LEVEL features) {
			device = dev;
			ctx = ctx_;
			level = features;
		}

		HRESULT create_D3D11_buffer(ID3D11Buffer** buf, const void* dat_in, unsigned int size, unsigned char item_size, unsigned int type, unsigned int flags, unsigned int CPU_flags, D3D11_USAGE usage) {
			D3D11_BUFFER_DESC buffer_description = { };
			buffer_description.ByteWidth = size;
			buffer_description.StructureByteStride = item_size;
			buffer_description.BindFlags = type;
			buffer_description.CPUAccessFlags = CPU_flags;
			buffer_description.MiscFlags = flags;
			buffer_description.Usage = usage;

			D3D11_SUBRESOURCE_DATA data_in = { };
			if (dat_in) {
				data_in.pSysMem = dat_in;
			}

			return device->CreateBuffer(&buffer_description, dat_in ? &data_in : nullptr, buf);
		}

		HRESULT compile_shader(LPCWSTR file_name, LPCSTR entrypoint) {
			ID3DBlob* error_blob;
			return D3DCompileFromFile(file_name, nullptr, nullptr, entrypoint, "cs_5_0", 0, 0, &shader_blob, &error_blob);
		}

		ID3DBlob* get_error_blob() {
			return error_blob;
		}

		ID3DBlob* get_shader_blob() {
			return shader_blob;
		}

		HRESULT create_UAV(ID3D11Resource* buf, ID3D11UnorderedAccessView** UAV, unsigned int size, DXGI_FORMAT format) {
			D3D11_UNORDERED_ACCESS_VIEW_DESC UAV_desc = { };
			UAV_desc.Format = format;
			UAV_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAV_desc.Buffer.FirstElement = 0;
			UAV_desc.Buffer.NumElements = size;

			return device->CreateUnorderedAccessView(buf, &UAV_desc, UAV);
		}

		template<typename T> T* retreive_D3D11_process(ID3D11Resource* CPU_buf, ID3D11Resource* GPU_buf) {
			D3D11_MAPPED_SUBRESOURCE map_r = { };
			ctx->CopyResource(CPU_buf, GPU_buf);
			HRESULT hr = ctx->Map(CPU_buf, 0, D3D11_MAP_READ, 0, &map_r);

			if (FAILED(hr)) {
				std::cout << "Mapping failed. HRESULT: " << std::hex << hr << "\n";
				return 0;
			}

			if (SUCCEEDED(hr)) {
				T* dat = (T*)map_r.pData;
				ctx->Unmap(CPU_buf, 0);

				return dat;
			}

			return 0;
		}
};
