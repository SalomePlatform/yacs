# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

class AdaptationError(TypeError):
        pass
class LiskovViolation(AdaptationError):
        pass

_adapter_factory_registry = {}

def registerAdapterFactory(objtype, protocol, factory):
        _adapter_factory_registry[objtype, protocol] = factory

def unregisterAdapterFactory(objtype, protocol):
        del _adapter_factory_registry[objtype, protocol]

def _adapt_by_registry(obj, protocol, alternate):
        factory = _adapter_factory_registry.get((type(obj), protocol))
        if factory is None:
            adapter = alternate
        else:
            adapter = factory(obj, protocol, alternate)
        if adapter is AdaptationError:
            raise AdaptationError,obj
        else:
            return adapter


def adapt(obj, protocol, alternate=AdaptationError):

        t = type(obj)

        # (a) first check to see if object has the exact protocol
        if t is protocol:
           return obj

        try:
            # (b) next check if t.__conform__ exists & likes protocol
            conform = getattr(t, '__conform__', None)
            if conform is not None:
                result = conform(obj, protocol)
                if result is not None:
                    return result

            # (c) then check if protocol.__adapt__ exists & likes obj
            adapt = getattr(type(protocol), '__adapt__', None)
            if adapt is not None:
                result = adapt(protocol, obj)
                if result is not None:
                    return result
        except LiskovViolation:
            pass
        else:
            # (d) check if object is instance of protocol
            try:
               if isinstance(obj, protocol):
                   return obj
            except:
               pass

        # (e) last chance: try the registry
        return _adapt_by_registry(obj, protocol, alternate)
