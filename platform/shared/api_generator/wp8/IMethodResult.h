#pragma once

namespace rhoruntime {

    public interface class IMethodResult
    {
    public:
        bool hasCallback();
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
		void set(bool res);
        void set(int64 res);
        void set(int res);
        void set(double res);
        void set(Platform::String^ res);
        void set(Windows::Foundation::Collections::IVectorView<Platform::String^>^ res);
        void set(Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ res);
    };

}
