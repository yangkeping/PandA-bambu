/*
 *
 *                   _/_/_/    _/_/   _/    _/ _/_/_/    _/_/
 *                  _/   _/ _/    _/ _/_/  _/ _/   _/ _/    _/
 *                 _/_/_/  _/_/_/_/ _/  _/_/ _/   _/ _/_/_/_/
 *                _/      _/    _/ _/    _/ _/   _/ _/    _/
 *               _/      _/    _/ _/    _/ _/_/_/  _/    _/
 *
 *             ***********************************************
 *                              PandA Project
 *                     URL: http://panda.dei.polimi.it
 *                       Politecnico di Milano - DEIB
 *                        System Architectures Group
 *             ***********************************************
 *              Copyright (C) 2022-2023 Politecnico di Milano
 *
 *   This file is part of the PandA framework.
 *
 *   The PandA framework is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/**
 * @file InterfaceInfer.hpp
 * @brief Load parsed protocol interface attributes
 *
 * @author Michele Fiorito <michele.fiorito@polimi.it>
 *
 */
#ifndef INTERFACE_INFER_HPP
#define INTERFACE_INFER_HPP

#include "application_frontend_flow_step.hpp"

/// utility includes
#include "refcount.hpp"

/// Included for interface_attributes enum
#include "hls_manager.hpp"

REF_FORWARD_DECL(tree_node);
CONSTREF_FORWARD_DECL(tree_node);
REF_FORWARD_DECL(tree_manipulation);
REF_FORWARD_DECL(tree_manager);
struct statement_list;
struct function_decl;
struct gimple_assign;
struct gimple_node;

#include "custom_set.hpp"
#include <list>
#include <set>

class InterfaceInfer : public ApplicationFrontendFlowStep
{
 private:
   enum class m_axi_type
   {
      none,
      direct,
      axi_slave
   };

   enum class datatype
   {
      generic,
      ac_type,
      real
   };

   struct interface_info
   {
      std::string name;
      unsigned alignment;
      unsigned long long bitwidth;
      datatype type;

      interface_info();

      void update(const tree_nodeRef& tn, std::string type_name, ParameterConstRef parameters);
   };

   bool already_executed;

   /**
    * Return the set of analyses in relationship with this design step
    * @param relationship_type is the type of relationship to be considered
    */
   const CustomUnorderedSet<std::pair<FrontendFlowStepType, FunctionRelationship>>
   ComputeFrontendRelationships(const DesignFlowStep::RelationshipType relationship_type) const override;

   void ComputeRelationships(DesignFlowStepSet& relationship,
                             const DesignFlowStep::RelationshipType relationship_type) override;

   void ChasePointerInterfaceRecurse(CustomOrderedSet<unsigned>& Visited, tree_nodeRef ptr_var,
                                     std::list<tree_nodeRef>& writeStmt, std::list<tree_nodeRef>& readStmt,
                                     interface_info& info);

   void ChasePointerInterface(tree_nodeRef ptr_var, std::list<tree_nodeRef>& writeStmt,
                              std::list<tree_nodeRef>& readStmt, interface_info& info);

   void setReadInterface(tree_nodeRef stmt, const std::string& arg_name, std::set<std::string>& operationsR,
                         bool commonRWSignature, tree_nodeConstRef interface_datatype,
                         const tree_manipulationRef tree_man, const tree_managerRef TM);

   void setWriteInterface(tree_nodeRef stmt, const std::string& arg_name, std::set<std::string>& operationsW,
                          bool commonRWSignature, tree_nodeConstRef interface_datatype,
                          const tree_manipulationRef tree_man, const tree_managerRef TM);

   void create_resource_Read_simple(const std::set<std::string>& operations, const std::string& arg_name,
                                    const interface_info& info, bool IO_port, unsigned int top_id) const;

   void create_resource_Write_simple(const std::set<std::string>& operations, const std::string& arg_name,
                                     const interface_info& info, bool IO_port, unsigned int top_id) const;

   void create_resource_array(const std::set<std::string>& operationsR, const std::set<std::string>& operationsW,
                              const std::string& bundle_name, const interface_info& info, unsigned int arraySize,
                              unsigned int top_id) const;

   void create_resource_m_axi(const std::set<std::string>& operationsR, const std::set<std::string>& operationsW,
                              const std::string& arg_name, const std::string& bundle_name, const interface_info& info,
                              m_axi_type mat, unsigned int top_id,
                              const std::map<interface_attributes, std::string>& bundle_attr_map) const;

   void create_resource(const std::set<std::string>& operationsR, const std::set<std::string>& operationsW,
                        const std::string& arg_name, const interface_info& info, const std::string& fname,
                        unsigned int top_id) const;

 public:
   /**
    * Constructor
    * @param AppM is the application manager
    * @param design_flow_manager is the design flow manager
    * @param _Param is the set of the parameters
    */
   InterfaceInfer(const application_managerRef AppM, const DesignFlowManagerConstRef design_flow_manager,
                  const ParameterConstRef parameters);

   /**
    * Destructor
    */
   ~InterfaceInfer() override;

   bool HasToBeExecuted() const override;

   void Initialize() override;

   /**
    * Execute this step
    * @return the exit status of this step
    */
   DesignFlowStep_Status Exec() override;
};

#endif
