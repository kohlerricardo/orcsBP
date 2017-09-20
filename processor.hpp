// ============================================================================
// ============================================================================
class processor_t {
    private:    
    
    
    public:

		// ====================================================================
		/// Methods
		// ====================================================================
		
		processor_t();
	    void allocate();
	    void clock();
		void statistics();

		// BTB attribute
		btb_t *btb;
		//BTB Methods
		void updateLruAll(uint32_t add);
		void replaceBTB(uint32_t address);
		uint32_t searchLine(uint32_t pc);
		uint32_t installLine(opcode_package_t instruction);
		inline uint32_t searchLru(btb_t *btb);
		// Statistics values
		uint32_t btbHits;
		uint32_t btbMiss;
		uint32_t branchTaken;
		uint32_t branchNotTaken;
		uint32_t branches;
		uint32_t BtMiss;
		uint32_t BntMiss;
		
		//others
		uint32_t index;
		uint32_t assoc;
		uint32_t has_branch;
		uint32_t nextInstruction;
		//plbt
		uint32_t predict;
		uint32_t oldAdd;
};
