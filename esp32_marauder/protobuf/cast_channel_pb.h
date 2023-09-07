/*
 *  Copyright (C) 2020-2022 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 *  <https://EmbeddedProto.com/license/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal address:
 *    Johan Huizingalaan 763a
 *    1066 VH, Amsterdam
 *    the Netherlands
 */

// This file is generated. Please do not edit!
#ifndef CAST_CHANNEL_H
#define CAST_CHANNEL_H

#include <cstdint>
#include "MessageInterface.h"
#include "WireFormatter.h"
#include "Fields.h"
#include "MessageSizeCalculator.h"
#include "ReadBufferSection.h"
#include "RepeatedFieldFixedSize.h"
#include "FieldStringBytes.h"
#include "Errors.h"
#include <limits>

// Include external proto definitions

namespace api {

class AuthChallenge final: public ::EmbeddedProto::MessageInterface
{
  public:
    AuthChallenge() = default;
    AuthChallenge(const AuthChallenge& rhs )
    {
    }

    AuthChallenge(const AuthChallenge&& rhs ) noexcept
    {
    }

    ~AuthChallenge() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
    };

    AuthChallenge& operator=(const AuthChallenge& rhs)
    {
      return *this;
    }

    AuthChallenge& operator=(const AuthChallenge&& rhs) noexcept
    {
      return *this;
    }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

    private:



};

class AuthError final: public ::EmbeddedProto::MessageInterface
{
  public:
    AuthError() = default;
    AuthError(const AuthError& rhs )
    {
      set_error_type(rhs.get_error_type());
    }

    AuthError(const AuthError&& rhs ) noexcept
    {
      set_error_type(rhs.get_error_type());
    }

    ~AuthError() override = default;

    enum class ErrorType : uint32_t
    {
      INTERNAL_ERROR = 0,
      NO_TLS = 1
    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      ERROR_TYPE = 1
    };

    AuthError& operator=(const AuthError& rhs)
    {
      set_error_type(rhs.get_error_type());
      return *this;
    }

    AuthError& operator=(const AuthError&& rhs) noexcept
    {
      set_error_type(rhs.get_error_type());
      return *this;
    }

    static constexpr char const* ERROR_TYPE_NAME = "error_type";
    inline void clear_error_type() { error_type_.clear(); }
    inline void set_error_type(const ErrorType& value) { error_type_ = value; }
    inline void set_error_type(const ErrorType&& value) { error_type_ = value; }
    inline const ErrorType& get_error_type() const { return error_type_.get(); }
    inline ErrorType error_type() const { return error_type_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((static_cast<ErrorType>(0) != error_type_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = error_type_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ERROR_TYPE), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::ERROR_TYPE:
            return_value = error_type_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_error_type();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::ERROR_TYPE:
          name = ERROR_TYPE_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

    private:


      EmbeddedProto::enumeration<ErrorType> error_type_ = static_cast<ErrorType>(0);

};

template<
    uint32_t AuthResponse_signature_LENGTH, 
    uint32_t AuthResponse_client_auth_certificate_LENGTH
>
class AuthResponse final: public ::EmbeddedProto::MessageInterface
{
  public:
    AuthResponse() = default;
    AuthResponse(const AuthResponse& rhs )
    {
      set_signature(rhs.get_signature());
      set_client_auth_certificate(rhs.get_client_auth_certificate());
    }

    AuthResponse(const AuthResponse&& rhs ) noexcept
    {
      set_signature(rhs.get_signature());
      set_client_auth_certificate(rhs.get_client_auth_certificate());
    }

    ~AuthResponse() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SIGNATURE = 1,
      CLIENT_AUTH_CERTIFICATE = 2
    };

    AuthResponse& operator=(const AuthResponse& rhs)
    {
      set_signature(rhs.get_signature());
      set_client_auth_certificate(rhs.get_client_auth_certificate());
      return *this;
    }

    AuthResponse& operator=(const AuthResponse&& rhs) noexcept
    {
      set_signature(rhs.get_signature());
      set_client_auth_certificate(rhs.get_client_auth_certificate());
      return *this;
    }

    static constexpr char const* SIGNATURE_NAME = "signature";
    inline void clear_signature() { signature_.clear(); }
    inline ::EmbeddedProto::FieldBytes<AuthResponse_signature_LENGTH>& mutable_signature() { return signature_; }
    inline void set_signature(const ::EmbeddedProto::FieldBytes<AuthResponse_signature_LENGTH>& rhs) { signature_.set(rhs); }
    inline const ::EmbeddedProto::FieldBytes<AuthResponse_signature_LENGTH>& get_signature() const { return signature_; }
    inline const uint8_t* signature() const { return signature_.get_const(); }

    static constexpr char const* CLIENT_AUTH_CERTIFICATE_NAME = "client_auth_certificate";
    inline void clear_client_auth_certificate() { client_auth_certificate_.clear(); }
    inline ::EmbeddedProto::FieldBytes<AuthResponse_client_auth_certificate_LENGTH>& mutable_client_auth_certificate() { return client_auth_certificate_; }
    inline void set_client_auth_certificate(const ::EmbeddedProto::FieldBytes<AuthResponse_client_auth_certificate_LENGTH>& rhs) { client_auth_certificate_.set(rhs); }
    inline const ::EmbeddedProto::FieldBytes<AuthResponse_client_auth_certificate_LENGTH>& get_client_auth_certificate() const { return client_auth_certificate_; }
    inline const uint8_t* client_auth_certificate() const { return client_auth_certificate_.get_const(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = signature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SIGNATURE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = client_auth_certificate_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CLIENT_AUTH_CERTIFICATE), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SIGNATURE:
            return_value = signature_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CLIENT_AUTH_CERTIFICATE:
            return_value = client_auth_certificate_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_signature();
      clear_client_auth_certificate();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SIGNATURE:
          name = SIGNATURE_NAME;
          break;
        case FieldNumber::CLIENT_AUTH_CERTIFICATE:
          name = CLIENT_AUTH_CERTIFICATE_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

    private:


      ::EmbeddedProto::FieldBytes<AuthResponse_signature_LENGTH> signature_;
      ::EmbeddedProto::FieldBytes<AuthResponse_client_auth_certificate_LENGTH> client_auth_certificate_;

};

template<
    uint32_t CastMessage_source_id_LENGTH, 
    uint32_t CastMessage_destination_id_LENGTH, 
    uint32_t CastMessage_namespace_LENGTH, 
    uint32_t CastMessage_payload_utf8_LENGTH, 
    uint32_t CastMessage_payload_binary_LENGTH
>
class CastMessage final: public ::EmbeddedProto::MessageInterface
{
  public:
    CastMessage() = default;
    CastMessage(const CastMessage& rhs )
    {
      set_protocol_version(rhs.get_protocol_version());
      set_source_id(rhs.get_source_id());
      set_destination_id(rhs.get_destination_id());
      set_namespace(rhs.get_namespace());
      set_payload_type(rhs.get_payload_type());
      set_payload_utf8(rhs.get_payload_utf8());
      set_payload_binary(rhs.get_payload_binary());
    }

    CastMessage(const CastMessage&& rhs ) noexcept
    {
      set_protocol_version(rhs.get_protocol_version());
      set_source_id(rhs.get_source_id());
      set_destination_id(rhs.get_destination_id());
      set_namespace(rhs.get_namespace());
      set_payload_type(rhs.get_payload_type());
      set_payload_utf8(rhs.get_payload_utf8());
      set_payload_binary(rhs.get_payload_binary());
    }

    ~CastMessage() override = default;

    enum class ProtocolVersion : uint32_t
    {
      CASTV2_1_0 = 0
    };

    enum class PayloadType : uint32_t
    {
      STRING = 0,
      BINARY = 1
    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      PROTOCOL_VERSION = 1,
      SOURCE_ID = 2,
      DESTINATION_ID = 3,
      NAMESPACE = 4,
      PAYLOAD_TYPE = 5,
      PAYLOAD_UTF8 = 6,
      PAYLOAD_BINARY = 7
    };

    CastMessage& operator=(const CastMessage& rhs)
    {
      set_protocol_version(rhs.get_protocol_version());
      set_source_id(rhs.get_source_id());
      set_destination_id(rhs.get_destination_id());
      set_namespace(rhs.get_namespace());
      set_payload_type(rhs.get_payload_type());
      set_payload_utf8(rhs.get_payload_utf8());
      set_payload_binary(rhs.get_payload_binary());
      return *this;
    }

    CastMessage& operator=(const CastMessage&& rhs) noexcept
    {
      set_protocol_version(rhs.get_protocol_version());
      set_source_id(rhs.get_source_id());
      set_destination_id(rhs.get_destination_id());
      set_namespace(rhs.get_namespace());
      set_payload_type(rhs.get_payload_type());
      set_payload_utf8(rhs.get_payload_utf8());
      set_payload_binary(rhs.get_payload_binary());
      return *this;
    }

    static constexpr char const* PROTOCOL_VERSION_NAME = "protocol_version";
    inline void clear_protocol_version() { protocol_version_.clear(); }
    inline void set_protocol_version(const ProtocolVersion& value) { protocol_version_ = value; }
    inline void set_protocol_version(const ProtocolVersion&& value) { protocol_version_ = value; }
    inline const ProtocolVersion& get_protocol_version() const { return protocol_version_.get(); }
    inline ProtocolVersion protocol_version() const { return protocol_version_.get(); }

    static constexpr char const* SOURCE_ID_NAME = "source_id";
    inline void clear_source_id() { source_id_.clear(); }
    inline ::EmbeddedProto::FieldString<CastMessage_source_id_LENGTH>& mutable_source_id() { return source_id_; }
    inline void set_source_id(const ::EmbeddedProto::FieldString<CastMessage_source_id_LENGTH>& rhs) { source_id_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<CastMessage_source_id_LENGTH>& get_source_id() const { return source_id_; }
    inline const char* source_id() const { return source_id_.get_const(); }

    static constexpr char const* DESTINATION_ID_NAME = "destination_id";
    inline void clear_destination_id() { destination_id_.clear(); }
    inline ::EmbeddedProto::FieldString<CastMessage_destination_id_LENGTH>& mutable_destination_id() { return destination_id_; }
    inline void set_destination_id(const ::EmbeddedProto::FieldString<CastMessage_destination_id_LENGTH>& rhs) { destination_id_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<CastMessage_destination_id_LENGTH>& get_destination_id() const { return destination_id_; }
    inline const char* destination_id() const { return destination_id_.get_const(); }

    static constexpr char const* NAMESPACE_NAME = "namespace";
    inline void clear_namespace() { namespace_.clear(); }
    inline ::EmbeddedProto::FieldString<CastMessage_namespace_LENGTH>& mutable_namespace() { return namespace_; }
    inline void set_namespace(const ::EmbeddedProto::FieldString<CastMessage_namespace_LENGTH>& rhs) { namespace_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<CastMessage_namespace_LENGTH>& get_namespace() const { return namespace_; }
    inline const char* name_space() const { return namespace_.get_const(); }

    static constexpr char const* PAYLOAD_TYPE_NAME = "payload_type";
    inline void clear_payload_type() { payload_type_.clear(); }
    inline void set_payload_type(const PayloadType& value) { payload_type_ = value; }
    inline void set_payload_type(const PayloadType&& value) { payload_type_ = value; }
    inline const PayloadType& get_payload_type() const { return payload_type_.get(); }
    inline PayloadType payload_type() const { return payload_type_.get(); }

    static constexpr char const* PAYLOAD_UTF8_NAME = "payload_utf8";
    inline void clear_payload_utf8() { payload_utf8_.clear(); }
    inline ::EmbeddedProto::FieldString<CastMessage_payload_utf8_LENGTH>& mutable_payload_utf8() { return payload_utf8_; }
    inline void set_payload_utf8(const ::EmbeddedProto::FieldString<CastMessage_payload_utf8_LENGTH>& rhs) { payload_utf8_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<CastMessage_payload_utf8_LENGTH>& get_payload_utf8() const { return payload_utf8_; }
    inline const char* payload_utf8() const { return payload_utf8_.get_const(); }

    static constexpr char const* PAYLOAD_BINARY_NAME = "payload_binary";
    inline void clear_payload_binary() { payload_binary_.clear(); }
    inline ::EmbeddedProto::FieldBytes<CastMessage_payload_binary_LENGTH>& mutable_payload_binary() { return payload_binary_; }
    inline void set_payload_binary(const ::EmbeddedProto::FieldBytes<CastMessage_payload_binary_LENGTH>& rhs) { payload_binary_.set(rhs); }
    inline const ::EmbeddedProto::FieldBytes<CastMessage_payload_binary_LENGTH>& get_payload_binary() const { return payload_binary_; }
    inline const uint8_t* payload_binary() const { return payload_binary_.get_const(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((static_cast<ProtocolVersion>(0) != protocol_version_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = protocol_version_.serialize_with_id(static_cast<uint32_t>(FieldNumber::PROTOCOL_VERSION), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = source_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SOURCE_ID), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = destination_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DESTINATION_ID), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = namespace_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAMESPACE), buffer, false);
      }

      if((static_cast<PayloadType>(0) != payload_type_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = payload_type_.serialize_with_id(static_cast<uint32_t>(FieldNumber::PAYLOAD_TYPE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = payload_utf8_.serialize_with_id(static_cast<uint32_t>(FieldNumber::PAYLOAD_UTF8), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = payload_binary_.serialize_with_id(static_cast<uint32_t>(FieldNumber::PAYLOAD_BINARY), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::PROTOCOL_VERSION:
            return_value = protocol_version_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SOURCE_ID:
            return_value = source_id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::DESTINATION_ID:
            return_value = destination_id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NAMESPACE:
            return_value = namespace_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::PAYLOAD_TYPE:
            return_value = payload_type_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::PAYLOAD_UTF8:
            return_value = payload_utf8_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::PAYLOAD_BINARY:
            return_value = payload_binary_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_protocol_version();
      clear_source_id();
      clear_destination_id();
      clear_namespace();
      clear_payload_type();
      clear_payload_utf8();
      clear_payload_binary();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::PROTOCOL_VERSION:
          name = PROTOCOL_VERSION_NAME;
          break;
        case FieldNumber::SOURCE_ID:
          name = SOURCE_ID_NAME;
          break;
        case FieldNumber::DESTINATION_ID:
          name = DESTINATION_ID_NAME;
          break;
        case FieldNumber::NAMESPACE:
          name = NAMESPACE_NAME;
          break;
        case FieldNumber::PAYLOAD_TYPE:
          name = PAYLOAD_TYPE_NAME;
          break;
        case FieldNumber::PAYLOAD_UTF8:
          name = PAYLOAD_UTF8_NAME;
          break;
        case FieldNumber::PAYLOAD_BINARY:
          name = PAYLOAD_BINARY_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

    private:


      EmbeddedProto::enumeration<ProtocolVersion> protocol_version_ = static_cast<ProtocolVersion>(0);
      ::EmbeddedProto::FieldString<CastMessage_source_id_LENGTH> source_id_;
      ::EmbeddedProto::FieldString<CastMessage_destination_id_LENGTH> destination_id_;
      ::EmbeddedProto::FieldString<CastMessage_namespace_LENGTH> namespace_;
      EmbeddedProto::enumeration<PayloadType> payload_type_ = static_cast<PayloadType>(0);
      ::EmbeddedProto::FieldString<CastMessage_payload_utf8_LENGTH> payload_utf8_;
      ::EmbeddedProto::FieldBytes<CastMessage_payload_binary_LENGTH> payload_binary_;

};

template<
    uint32_t DeviceAuthMessage_response_AuthResponse_signature_LENGTH, 
    uint32_t DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH
>
class DeviceAuthMessage final: public ::EmbeddedProto::MessageInterface
{
  public:
    DeviceAuthMessage() = default;
    DeviceAuthMessage(const DeviceAuthMessage& rhs )
    {
      set_challenge(rhs.get_challenge());
      set_response(rhs.get_response());
      set_error(rhs.get_error());
    }

    DeviceAuthMessage(const DeviceAuthMessage&& rhs ) noexcept
    {
      set_challenge(rhs.get_challenge());
      set_response(rhs.get_response());
      set_error(rhs.get_error());
    }

    ~DeviceAuthMessage() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      CHALLENGE = 1,
      RESPONSE = 2,
      ERROR = 3
    };

    DeviceAuthMessage& operator=(const DeviceAuthMessage& rhs)
    {
      set_challenge(rhs.get_challenge());
      set_response(rhs.get_response());
      set_error(rhs.get_error());
      return *this;
    }

    DeviceAuthMessage& operator=(const DeviceAuthMessage&& rhs) noexcept
    {
      set_challenge(rhs.get_challenge());
      set_response(rhs.get_response());
      set_error(rhs.get_error());
      return *this;
    }

    static constexpr char const* CHALLENGE_NAME = "challenge";
    inline void clear_challenge() { challenge_.clear(); }
    inline void set_challenge(const AuthChallenge& value) { challenge_ = value; }
    inline void set_challenge(const AuthChallenge&& value) { challenge_ = value; }
    inline AuthChallenge& mutable_challenge() { return challenge_; }
    inline const AuthChallenge& get_challenge() const { return challenge_; }
    inline const AuthChallenge& challenge() const { return challenge_; }

    static constexpr char const* RESPONSE_NAME = "response";
    inline void clear_response() { response_.clear(); }
    inline void set_response(const AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH>& value) { response_ = value; }
    inline void set_response(const AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH>&& value) { response_ = value; }
    inline AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH>& mutable_response() { return response_; }
    inline const AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH>& get_response() const { return response_; }
    inline const AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH>& response() const { return response_; }

    static constexpr char const* ERROR_NAME = "error";
    inline void clear_error() { error_.clear(); }
    inline void set_error(const AuthError& value) { error_ = value; }
    inline void set_error(const AuthError&& value) { error_ = value; }
    inline AuthError& mutable_error() { return error_; }
    inline const AuthError& get_error() const { return error_; }
    inline const AuthError& error() const { return error_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = challenge_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CHALLENGE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = response_.serialize_with_id(static_cast<uint32_t>(FieldNumber::RESPONSE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = error_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ERROR), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::CHALLENGE:
            return_value = challenge_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::RESPONSE:
            return_value = response_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::ERROR:
            return_value = error_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_challenge();
      clear_response();
      clear_error();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::CHALLENGE:
          name = CHALLENGE_NAME;
          break;
        case FieldNumber::RESPONSE:
          name = RESPONSE_NAME;
          break;
        case FieldNumber::ERROR:
          name = ERROR_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

    private:


      AuthChallenge challenge_;
      AuthResponse<DeviceAuthMessage_response_AuthResponse_signature_LENGTH, DeviceAuthMessage_response_AuthResponse_client_auth_certificate_LENGTH> response_;
      AuthError error_;

};

} // End of namespace api
#endif // CAST_CHANNEL_H